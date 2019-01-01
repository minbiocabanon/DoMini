//----------------------------------------------------------------------
//!\file		planning.c
//!\date		2015
//!\author		Minbiocabanon
//!\brief		Programme qui cree les tables de planning pour l'annee
//----------------------------------------------------------------------
 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <mysql/mysql.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h> //gestion des repertoire
#include <signal.h>
#include <time.h>

#include <string.h>
#include <stddef.h>
#include "passwd.h"

#define _POSIX_SOURCE 1 // POSIX compliant source
#define FALSE 0
#define TRUE 1

//table calendrier
#define ROW_SAISON 0
#define ROW_TYPE_JOUR 1
#define ROW_UNIX_TIME 2

//table calendrier_saison
#define ROW_CONSIGNE_TEMPERATURE 0

//table calendrier_type_jour
#define ROW_HEURE_DEBUT 0
#define ROW_HEURE_FIN 1
#define ROW_PRIORITE 2

//Constante pour le planning
#define	INTERVALLE_PLANNING	1799	//en secondes : 29:59 = 1799

// prototypes des fonctions utilisées
void sigfun(int sig);
int init_mysql(void);
int recup_info_jour(unsigned int nId);
int read_consigne_chauffe(char *inf_saison);
unsigned int calcul_nb_jour(unsigned int annee);
unsigned int calcul_t0(unsigned int annee);
int planning_vierge(unsigned int annee);
int affectation_jour_planning(void);

// Declaration des variables 
int keep_looping = TRUE;
unsigned int nNbJour = 0 ;
char stconsigne_temperature[] = "19.0";	//consigne de température, 19°C par défaut dans un string pour la récup depuis mysql

typedef struct stMydata stMydata;
struct stMydata{
	unsigned int annee_planning;	
	unsigned int nIdJour ; 			//Id du jour en cours de traitement,  0 < nIdJour <= 365
	unsigned int nUnixTime; 		//date du jour au format Unix Time
	float fconsigne_temperature;	//consigne de température, 19°C par défaut. 
	char inf_saison[2];  			// saison "A" par défaut -> automatique
	char inf_type_jour[2];	 		// type de jour par défaut "T" = travail ... bah oué
	char stheure_debut[10]; 			//heure de début du créneau de la tache à faire
	char stheure_fin[10]; 			//heure de début du créneau de la tache à faire
	char stheure_priorite[2]; 
};
//déclaration de la structure
stMydata stJourTraite ;

// On déclare un pointeur long sur un non signé pour y stocker la taille des valeurs
unsigned long *lengths;

// pour la gestion de l'heure/date
time_t t;
struct tm *local;

// Déclaration du pointeur de structure de type MYSQL
MYSQL *conn;
MYSQL_RES *result = NULL;
MYSQL_ROW row = NULL;
MYSQL_RES *result2 = NULL;
MYSQL_ROW row2 = NULL;
// nom de la table mysql concernée
char *table = "calendrier";


//----------------------------------------------------------------------
//!\brief           trap du control C
//!\param[in]     sig paramètre qui ne sert à rien
//----------------------------------------------------------------------
void sigfun(int sig) {
    printf("You have pressed Ctrl-C , aborting!\n");
    keep_looping = FALSE;
	// fermeture de MySQL
	mysql_close(conn);
    exit(0);
}


//----------------------------------------------------------------------
//!\brief           ouverture d'une connexion à la bdd
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int init_mysql(void){

  printf("With MySQL support\n");

  //Initialisation liaison avec mysql
  conn = mysql_init(NULL);
    
  // Connexion à la BDD
  printf("Connexion a la BDD\n");
  if(!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) {
    fprintf(stderr, "%s\n", mysql_error(conn));
    return(1);
  }
  
  // Envoi de la requete de sélection de la base à utiliser
  printf("Envoi de la requete USE DOMOTIQUE\n");
  if (mysql_query(conn, "USE domotique")) {
    fprintf(stderr, "%s\n", mysql_error(conn));
    return(1);
  }
  // s'il n'y a pas d'erreurs, tout est ok, on le dit
  return(0);
}


//----------------------------------------------------------------------
//!\brief           On vide la table 'calendrier_30min' pour préparer la future table
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int vider_table_jour(void){

	// Preparation de la requete MySQL
	// on ne récupère que les infos du jour en cours
	sprintf(query, "TRUNCATE TABLE `calendrier_30min`");

	// envoi de la requete
	printf("\nEnvoi de la requete : %s", query);
	if (mysql_query(conn, query)) {
	// si la requete echoue on retourne 1
	fprintf(stderr, "%s\n", mysql_error(conn));
	return(1);
	}

	// on libère la requête
	mysql_free_result(result);

	fflush(stdout);
	return(0);
}

//----------------------------------------------------------------------
//!\brief         On supprime de la table 'calendrier' les infos de l'annee precedente
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int supprimer_calendrier_prec(void){

	// Preparation de la requete MySQL
	// on supprimer de la table 'calendrier' toutes les infos de l'annee precedente, sinon les id ne vont pas être alignés (id = 1 -> debut de l'annee)
	sprintf(query, "DELETE FROM `calendrier` where YEAR(`date`) = %d", (stJourTraite.annee_planning - 1));

	// envoi de la requete
	printf("\nEnvoi de la requete : %s", query);
	if (mysql_query(conn, query)) {
	// si la requete echoue on retourne 1
	fprintf(stderr, "%s\n", mysql_error(conn));
	return(1);
	}

	// on libère la requête
	mysql_free_result(result);

	fflush(stdout);
	return(0);
}

//----------------------------------------------------------------------
//!\brief          On calcul avec une requete MySQL le nombre de jour qui compose l'année à venir
//!\param[in]  		Anne pour laquelle effectuer la requete
//!\return        retour dans variable globale nNbJour
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
unsigned int calcul_nb_jour(unsigned int annee){

	// Preparation de la requete MySQL
	// on ne récupère le nombre de jours pour l'année en cours
	sprintf(query, "select to_days('%d-12-31') - to_days('%d-01-01') +1  as days_left;", annee, annee);

	// envoi de la requete
	printf("\nEnvoi de la requete : %s", query);
	if (mysql_query(conn, query)) {
		// si la requete echoue on retourne 1
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
	}

	// la requête s'est bien passée, on rend le jeu de résultats disponible via le pointeur result
	printf("\nRecuperation des donnees");
	result = mysql_use_result(conn);

	// on récupère le résultat (une seule ligne en principe... si la requête est bien faite)
	row = mysql_fetch_row(result);

	//création de la variable string qui va contenir le mode contenu dans la base
	// on converti le string en float
	nNbJour = atoi(row[0]);

	// on affiche la valeur de la consigne de température pour le type de saison récupéré
	printf ("\nNb de jour cette annee : %d \n", nNbJour);
  
	// on libère la requête
	mysql_free_result(result);
	return(0);
}

//----------------------------------------------------------------------
//!\brief           Calcule le unixtime du 01-01 00:00:00 de l'année en cours
//!\param[in]  		Anne pour laquelle effectuer la requete
//!\return        	unixtime
//----------------------------------------------------------------------
unsigned int calcul_t0(unsigned int annee){

	//variable index du temps
	unsigned int nt0 = 0;
	
	//on récupère le unixtime du 01-01 00:00 de l'année passee en paramètre
	// ne sachant pas le faire en C (trop compliqué de manipuler le temps en C !!!) je le fais avec une requet SQL bien plus simple
	// Preparation de la requete MySQL
	sprintf(query, "SELECT UNIX_TIMESTAMP('%d-01-01 00:00:00:');", annee);

	// envoi de la requete
	printf("\nEnvoi de la requete : %s", query);
	if (mysql_query(conn, query)) {
		// si la requete echoue on retourne 1
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
	}

	// la requête s'est bien passée, on rend le jeu de résultats disponible via le pointeur result
	printf("\nRecuperation des donnees");
	result = mysql_use_result(conn);

	// on récupère le résultat (une seule ligne en principe... si la requête est bien faite)
	row = mysql_fetch_row(result);

	//création de la variable string qui va contenir le mode contenu dans la base
	// on converti le string en float
	nt0 = atoi(row[0]);

	// on affiche la valeur de la consigne de température pour le type de saison récupéré
	printf ("\nUnixTime du T0 : %d \n", nt0);
  
	// on libère la requête
	mysql_free_result(result);
	return(nt0);
}


//----------------------------------------------------------------------
//!\brief           On créé le planning vierge pour toute l'année
//!\param[in]  		Anne pour laquelle effectuer la requete
//!\return        	1 si erreur, 0 sinon
//----------------------------------------------------------------------
int planning_vierge(unsigned int annee){

	//variable pour l'incrémente des tranches horaires, on va fonctionner en unixtime depuis 00:00 du jour de l'an, donc on va ajouter 20 minutes
	// on initialise avec le t0 à savoir le 01-01 00:00:00 de l'année passee en parametre
	unsigned int nIndexTemps = calcul_t0(annee);
	unsigned int nIndexTemps_fin  = nIndexTemps + INTERVALLE_PLANNING;
	unsigned int crenau_30min = 1;
	//Pour chaque jour de l'année
	// on fait nNbJour * 48 , car il y 48 créneaux de 30 minutes par jour.
	printf("\nCreation de la table planning vierge... patientez\n");
	fflush(stdout);
	for(crenau_30min = 1; crenau_30min < (nNbJour * 48); crenau_30min++){
		// Preparation de la requete MySQL
		sprintf(query, "INSERT INTO `domotique`.`calendrier_30min` (`id`, `date`, `heure_debut`, `heure_fin`, `temperature`, `priorite`) VALUES (NULL , FROM_UNIXTIME(%d,'%%Y-%%m-%%d'), FROM_UNIXTIME(%d,'%%H:%%i:%%s'), FROM_UNIXTIME(%d ,'%%H:%%i:%%s'), 0.0, 0);", nIndexTemps, nIndexTemps, nIndexTemps_fin);
		
		// envoi de la requete
		//printf("\nEnvoi de la requete : %s", query);
		if (mysql_query(conn, query)) {
			// si la requete echoue on retourne 1
			fprintf(stderr, "%s\n", mysql_error(conn));
			return(1);
		}
		//on incrémente  le temps  (nIndexTemps en secondes)
		nIndexTemps = nIndexTemps + INTERVALLE_PLANNING + 1; // on ajoute 1 seconde pour passer à la minute suivante
		nIndexTemps_fin = nIndexTemps + INTERVALLE_PLANNING;
	}
	
	printf("\nPlanning vierge cree pour %d creneaux de 30 minutes", crenau_30min);
	//le planning vierge a été généré, on retourne 0
	return(0);
}

//----------------------------------------------------------------------
//!\brief           On affecte chaque jour du planning avec les paramètres caractéristiques du  type de jour et saison
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int affectation_jour_planning(void){

	

	//pour chaque jour de l'année, défini par un l'Id=1 à l'Id=nNbJour (365)
	for(stJourTraite.nIdJour = 1; stJourTraite.nIdJour <= nNbJour ; stJourTraite.nIdJour++){
	
		//récupérer le type de jour et saison
		recup_info_jour(stJourTraite.nIdJour);	
		
		//A partir de la saison , on récupére la température de chauffe
		read_consigne_chauffe(stJourTraite.inf_saison);
		
		unsigned int nPrio = 1;
		
		// Maintenant, on va traiter chaque créneau de chauffe du type de jour , puis remplir la table planning en conséquence (température de consigne à mettre à jour dans les créneau concernés)	
		// Preparation de la requete MySQL
		// on ne récupère que les infos du jour en cours
		sprintf(query, "SELECT `heure_debut`, `heure_fin`, `priorite` FROM `calendrier_type_jour` WHERE `type_jour` = '%s'", stJourTraite.inf_type_jour);

		// envoi de la requete
		printf("\n\nEnvoi de la requete : %s", query);
		if (mysql_query(conn, query)) {
			// si la requete echoue on retourne 1
			fprintf(stderr, "%s\n", mysql_error(conn));
			return(1);
		}

		// la requête s'est bien passée, on rend le jeu de résultats disponible via le pointeur result
		printf("\nRecuperation des donnees pour le type de jour");
		result = mysql_store_result(conn);
		
		while ((row = mysql_fetch_row(result))) {
			// on stocke la taille  dans le pointeur
			lengths = mysql_fetch_lengths(result);
			// on récupère l'heure de début du créneau de l'action a effectuer
			sprintf(stJourTraite.stheure_debut,"%.*s", (char) lengths[ROW_HEURE_DEBUT], row[ROW_HEURE_DEBUT]);
			// on récupère l'heure de fin du créneau de l'action a effectuer
			sprintf(stJourTraite.stheure_fin,"%.*s", (char) lengths[ROW_HEURE_FIN], row[ROW_HEURE_FIN]);
			// on récupère la priorité 
			sprintf(stJourTraite.stheure_priorite,"%.*s", (char) lengths[ROW_PRIORITE], row[ROW_PRIORITE]);						
			// send SQL query 
			// Exemple , ajout condition  AND addtime( `date` , `heure_fin` ) > addtime( `date` , `heure_debut` ) pour éviter que le créneau 23:30:00 - 00:00:00 soit compté (minuit étant plus petit que 23:30, on écrirait une consigne de température!)
			// SELECT `id` , `date` , `heure_debut` , `heure_fin` 
			// FROM `calendrier_30min` 
			// WHERE `date` = '2012-11-05'
			// AND `heure_debut` >= '18:00:00'
			// AND `heure_fin` <= '22:00:00'
			// AND addtime( `date` , `heure_fin` ) > addtime( `date` , `heure_debut` )
			sprintf(query, "SELECT `id` FROM `calendrier_30min` WHERE `date` = FROM_UNIXTIME( %d, '%%Y-%%m-%%d' ) AND `heure_debut` >= '%s' AND `heure_fin` <= '%s' AND addtime( `date` , `heure_fin` ) > addtime( `date` , `heure_debut` );", stJourTraite.nUnixTime, stJourTraite.stheure_debut, stJourTraite.stheure_fin);
			
			// envoi de la requete
			printf("\nEnvoi de la requete : %s", query);
			if (mysql_query(conn, query)) {
				// si la requete echoue on retourne 1
				fprintf(stderr, "%s\n", mysql_error(conn));
				return(1);
			}
			
			// la requête s'est bien passée, on rend le jeu de résultats disponible via le pointeur result2
			printf("\nRecuperation de l'id pour le creneau");
			result2 = mysql_store_result(conn);
			
			unsigned int nIdCreneau = 0;
			
			while ((row2 = mysql_fetch_row(result2))) {
				// on récupère l'id du créneau horaire
				nIdCreneau = atoi(row2[0]);				
				// send SQL query 
				sprintf(query, "UPDATE `domotique`.`calendrier_30min` SET `temperature` = '%.1f', `priorite` = '%d' WHERE `calendrier_30min`.`id` = %d;", stJourTraite.fconsigne_temperature, nPrio, nIdCreneau);
				printf("\n\nMise à jour de la temperature pour l'id : %s", query);
				if (mysql_query(conn, query)) {
					fprintf(stderr, "%s\n", mysql_error(conn));
					return(1);
				}
			}
			// on libère la requête
			mysql_free_result(result2);
			
			nPrio++;
		}
		// on libère la requête
		mysql_free_result(result);
	}

	//l'affectation s'est bien passé, on retourne 0
	return(0);
}



//----------------------------------------------------------------------
//!\brief		récupération des infos du jour dans la table calendrier (type de jour et saison)
//!\param[in]	Id du jour pour lequel il faut trouver la saison et le type de jour
//!\return		1 si erreur, 0 sinon
//----------------------------------------------------------------------
int recup_info_jour(unsigned int nId){

	// Preparation de la requete MySQL
	// on ne récupère que les infos du jour en cours
	sprintf(query, "SELECT `saison` , `type_jour` , UNIX_TIMESTAMP( `date` ) FROM `calendrier` WHERE `id` = %d", nId);

	// envoi de la requete
	printf("\n\nEnvoi de la requete : %s", query);
	if (mysql_query(conn, query)) {
		// si la requete echoue on retourne 1
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
	}

	// la requête s'est bien passée, on rend le jeu de résultats disponible via le pointeur result
	printf("\nRecuperation des donnees");
	result = mysql_use_result(conn);

	// on récupère le nombre de champs
	// unsigned int num_champs = mysql_num_fields(result);
	// printf("Recuperation du nombre de champs de la table: %d\n",num_champs);

	// on récupère le résultat (une seule ligne en principe... si la requête est bien faite)
	row = mysql_fetch_row(result);

	// on stocke la taille  dans le pointeur
	lengths = mysql_fetch_lengths(result);

	// on récupère la saison du jour en cours
	sprintf(stJourTraite.inf_saison,"%.*s", (char) lengths[ROW_SAISON], row[ROW_SAISON]); 
	// on affiche cette date (au format UNIX) pour la comparaison future avec la date courante
	printf ("\ninf_saison : %s \n", stJourTraite.inf_saison);

	// on récupère le type de jour  du jour en cours
	sprintf(stJourTraite.inf_type_jour,"%.*s", (char) lengths[ROW_TYPE_JOUR], row[ROW_TYPE_JOUR]);
	// on affiche le type de jour du jour en cours
	printf ("stJourTraite.inf_type_jour : %s \n", stJourTraite.inf_type_jour);

	// on récupère l'unixtime du jour en cours
	stJourTraite.nUnixTime = atoi(row[ROW_UNIX_TIME]);
	// on affiche le type de jour du jour en cours
	printf ("stJourTraite.nUnixTime : %d \n", stJourTraite.nUnixTime);

	// on libère la requête
	mysql_free_result(result);
	return(0);
}

//----------------------------------------------------------------------
//!\brief		Dans la table saison,  requête pour récupérer la consigne de chauffage en fonction de la saison
//!\param		saison (string) pour laquelle on veut la consigne de chauffage
//!\return		1 si erreur, 0 sinon
//----------------------------------------------------------------------
int read_consigne_chauffe(char *inf_saison){
	// Preparation de la requete MySQL
	// on ne récupère que les infos du jour en cours
	sprintf(query, "SELECT `consigne_temperature` FROM `calendrier_saison` WHERE `type` = '%s'", inf_saison);

	// envoi de la requete
	printf("\n\nEnvoi de la requete : %s", query);
	if (mysql_query(conn, query)) {
		// si la requete echoue on retourne 1
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
	}

	// la requête s'est bien passée, on rend le jeu de résultats disponible via le pointeur result
	printf("\nRecuperation des donnees");
	result = mysql_use_result(conn);

	// on récupère le résultat (une seule ligne en principe... si la requête est bien faite)
	row = mysql_fetch_row(result);

	// on stocke la taille  dans le pointeur
	lengths = mysql_fetch_lengths(result);

	// on récupère la consigne de température
	sprintf(stconsigne_temperature,"%.*s", (char) lengths[ROW_CONSIGNE_TEMPERATURE], row[ROW_CONSIGNE_TEMPERATURE]);

	// on converti le string en float
	stJourTraite.fconsigne_temperature = atof(stconsigne_temperature);

	// on affiche la valeur de la consigne de température pour le type de saison récupéré
	printf ("\nstJourTraite.consigne température : %.1f \n", stJourTraite.fconsigne_temperature);

	// on libère la requête
	mysql_free_result(result);
	
	return(0);
}


//***********************************************
//----------------------------------------------------------------------
//!\brief		le main !
//!\input		Année pour laquelle il faut créer le planning; si NULL alors on prend l'année courante
//----------------------------------------------------------------------
//***********************************************
int main(int argc, char *argv[]) {  
  
	// Rappel
	printf("Arg : Annee pour laquelle il faut creer le planning\n Exemple : \n $ ./planning 2020\n");

	//***************************
	//* INITIALISATION
	//***************************
	printf("Installing signal handler...");
	(void) signal(SIGINT, sigfun);
	printf("done\n");


	//initialisation BDD
	if(init_mysql() == 1) {
	// erreur de liaison avec la bdd
	// prévoir un msg de log vers un fichier de log
	exit(1);
	}
	
	//***************************************************************************
	//* PROGRAMME PRINCIPAL 
	//***************************************************************************

	//lecture de la date/heure courante
	// t = nb de secondes depuis le 1er janvier 1970
	t = time(NULL);  
	// conversion en heure locale
	local = localtime(&t); 
	printf("Heure et date courante : %s\n", asctime(local));

	// si l'utilisateur n'a pas passée l'annee en argument
	if(argc != 2) {
		// on détermine l'année en cours
		stJourTraite.annee_planning = local->tm_year + 1900;
		printf("\n Pas d'argument, on prend l'année  en cours : %d", stJourTraite.annee_planning);
	}
	else{
		//sinon on prend l'annee passe en arguments
		stJourTraite.annee_planning = atoi(argv[1]);
		printf("on prend l'aneee  en argument : %d", stJourTraite.annee_planning);
	}
	
	
	//on commence par vider la table , l'année précédent étant passé, place à l'aveniiiiiiiiiiir
	// requete de la mort qui vide la une table : TRUNCATE TABLE `calendrier_30min`
	if(vider_table_jour() == 1) {
		// erreur sur la recuperation des messages
		printf("\nERREUR dans la fonction qui vide la table du jour \n\n");
		// prévoir un msg de log vers un fichier de log
	exit(1);
	}

	// requete qui supprime de la table 'calendrier' les infos de l'annee precedente
	if(supprimer_calendrier_prec() == 1) {
		// erreur sur la recuperation des messages
		printf("\nERREUR dans la fonction qui supprime les infos de l'année %d \n\n", (stJourTraite.annee_planning - 1));
		// prévoir un msg de log vers un fichier de log
	exit(1);
	}	
	

	//on calcule le nombre de jour pour l'année à venir
	if(calcul_nb_jour(stJourTraite.annee_planning) == 1) {
		// erreur sur la recuperation des messages
		// prévoir un msg de log vers un fichier de log
		exit(1);
	}

	//Création du planning vierge
	if(planning_vierge(stJourTraite.annee_planning) == 1) {
		// erreur sur la recuperation des messages
		// prévoir un msg de log vers un fichier de log
		exit(1);
	}
	
	//On affecte a chaque jour du calendrier les consignes de chauffe pour chaque tranche horaire
	if(affectation_jour_planning() == 1) {
		// erreur sur la recuperation des messages
		// prévoir un msg de log vers un fichier de log
		exit(1);
	}

	// fermeture de MySQL
	mysql_close(conn);

	printf("\nEXIT \n\n");

	//exit(0);
}