//----------------------------------------------------------------------
//!\file		gestion_vr.c
//!\date
//!\author		Minbiocabanon
//!\brief		Programme qui décide de la position des VR... non mais!
//----------------------------------------------------------------------

 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
//#define WITHMYSQL
#ifdef WITHMYSQL
  #include <mysql/mysql.h>
#endif
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h> //gestion des repertoire
#include <signal.h>
#include <time.h>
#include <syslog.h>
#include <string.h>
#include <stddef.h>
#include "gestion_vr.h"
#include "passwd.h"

#define _POSIX_SOURCE 1 // POSIX compliant source
#define FALSE 0
#define TRUE 1

//table calendrier
#define row_id 0
#define row_date 1
#define row_saison 2
#define row_type_jour 3


//table calendrier_saison
#define row_consigne_temperature 2
#define row_flux 1

//table calendrier_type_jour
#define row_id_etat_VR 0
#define row_date 1
#define row_mode_VR 2
#define row_etat_bureau 3
#define row_etat_salon 4
#define row_etat_chm 5
#define row_etat_chjf 6

//table mode domoitique
#define row_mode_domotique 0

#define TIMEOUTMSG 60 //en secondes

#define FLUX_MAX_ETE 500
#define FLUX_MIN_HIVER 400

	// Declaration des variables 
float flux_solaire;
char saison[] = "X"; 	//saison du jour en cours
char type[] = "X"; 		//type du jour en cours
int keep_looping = TRUE;
int fd;

// On déclare un pointeur long sur un non signé pour y stocker la taille des valeurs
unsigned long *lengths;

// pour la gestion de l'heure/date
time_t t;
struct tm *local;
unsigned int time_unix_msg;
char log_name[] = "volet_roulants";

// Déclaration du pointeur de structure de type MYSQL
MYSQL *conn;
MYSQL_RES *result = NULL;
MYSQL_ROW row = NULL;
// nom de la table mysql concernée
char *table = "voletroulant_statut";


//----------------------------------------------------------------------
//!\brief           trap du control C
//!\param[in]     sig paramètre qui ne sert à rien
//----------------------------------------------------------------------
void sigfun(int sig) {
    printf("You have pressed Ctrl-C , aborting!\n");
    keep_looping = FALSE;
	syslog(LOG_DEBUG, "You have pressed Ctrl-C , aborting!");
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
  syslog(LOG_DEBUG, "Connexion a la BDD");
  if(!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) {
    fprintf(stderr, "%s\n", mysql_error(conn));
    return(1);
  }
  
  // Envoi de la requete de sélection de la base à utiliser
  syslog(LOG_DEBUG, "Envoi de la requete USE DOMOTIQUE");
  if (mysql_query(conn, "USE domotique")) {
    fprintf(stderr, "%s\n", mysql_error(conn));
    return(1);
  }
  // s'il n'y a pas d'erreurs, tout est ok, on le dit
  return(0);
}


//----------------------------------------------------------------------
//!\brief           récupération de la température de consigne
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int recup_temperature(char *saison){

	//récupération de la température de consigne
	sprintf(query, "SELECT `consigne_temperature`, `flux solaire` FROM calendrier_saison WHERE type = \"%s\"", saison);
	if (mysql_query(conn, query)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
    }
		
	result = mysql_use_result(conn);
	
	//si la requete a retourné une ligne non nulle
	// on lit la ligne demandée
	if (row = mysql_fetch_row(result)){
		//on converti la chaine en float
		stDonnees.consigne_temperat = atof(row[0]);
		syslog(LOG_DEBUG, "Consigne de temperature recuperee:%.2f", stDonnees.consigne_temperat);
		
		// on récupère le flux
		stDonnees.flux_solaire_trig = atoi(row[row_flux]);
		// on affiche le type de jour du jour en cours
		syslog(LOG_DEBUG, "flux_solaire_trig : %d", stDonnees.flux_solaire_trig);
	}else{
		// euh... je réfléchis
		syslog(LOG_ERR, "ERREUR : PAS DE CONSIGNE DE TEMPERATURE TROUVEE");
		stDonnees.consigne_temperat = 0; 
	}
	
	// on libère la requête
	mysql_free_result(result);  
	
	//récupération de la température actuelle (valeur la plus récente des 15 dernières minutes)
	sprintf(query, "SELECT ana1, date_time FROM analog2 WHERE date_time >= SUBTIME( NOW( ) , '00:15:00' ) ORDER BY date_time LIMIT 0 , 1");
	if (mysql_query(conn, query)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
    }
		
	result = mysql_use_result(conn);
	
	//si la requete a retourné une ligne non nulle
	// on lit la ligne demandée
	if (row = mysql_fetch_row(result)){
		//on converti la chaine en float
		stDonnees.temp_int = atof(row[0]);
		#if _DEBUG_
		syslog(LOG_DEBUG, "temperature int. courante :%.2f", stDonnees.temp_int);
	
		fflush(stdout);
		#endif  
	}else{
		// euh... je réfléchis
		syslog(LOG_ERR, "ERREUR : TEMPERATURE INTERIEURE N'EST PAS RECENTE");
		stDonnees.temp_int = 0; 
	}
	
	// on libère la requête
	mysql_free_result(result);  
		
	//on regarde si la température de consigne est atteinte (dépassée) ou non
	if (stDonnees.temp_int > stDonnees.consigne_temperat)
			//si oui, on lève le flag
			stDonnees.flag_temperature = 1;	
	else	//si oui, on met le flag à 0
			stDonnees.flag_temperature = 0;
			
	fflush(stdout);
	return(0);
}
//----------------------------------------------------------------------
//!\brief		Calcul l'état des vr à partir des conditions et d'une table de vérité
//!\return		1 si erreur, 0 sinon
//----------------------------------------------------------------------
int etat_vr_mode_auto(void){

	// Preparation de la requete MySQL
	// on ne récupère l'état des VR en fonction des condition d'entrées
	sprintf(query, "SELECT `VOLET BUREAU`,`VOLET SALON`,`VOLET CHM`,`VOLET CHJF` FROM `voletroulant_table_verite` WHERE `saison`= \"%s\" AND `flux solaire` = %d AND `jour/nuit`= %d  AND `Temperature consigne`=%d", saison, stDonnees.flag_flux, stDonnees.flag_jour, stDonnees.flag_temperature);

	// envoi de la requete
	syslog(LOG_DEBUG, "Envoi de la requete : %s", query);
	if (mysql_query(conn, query)) {
		// si la requete echoue on retourne 1
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
	}

	// la requête s'est bien passée, on rend le jeu de résultats disponible via le pointeur result
	syslog(LOG_DEBUG, "Recuperation des donnees");
	result = mysql_use_result(conn);
	
	//si la requete a retourné une ligne non nulle
	// on lit la ligne demandée
	if (row = mysql_fetch_row(result)){
		// on stocke la taille  dans le pointeur
		lengths = mysql_fetch_lengths(result);
		
		//déclaration de variables temporaires en char pour récupérer les données mysql
		char charmode[]="0", charetatvrbureau[]="0", charetatvrsalon[]="0", charetatvrchm[]="0", charetatvrchjf[]="0";

		// on récupère l'état du volet du bureau
		sprintf(charetatvrbureau,"%.*s", (char) lengths[0], row[0]);
		//on converti en int
		stEtatVR.etat_vr_bureau = atoi(charetatvrbureau);
		// on affiche le type de jour du jour en cours
		syslog(LOG_DEBUG, "etat vr bureau : %d", stEtatVR.etat_vr_bureau);
		
		// on récupère l'état du volet du salon
		sprintf(charetatvrsalon,"%.*s", (char) lengths[1], row[1]);
		//on converti en int
		stEtatVR.etat_vr_salon = atoi(charetatvrsalon);
		// on affiche le type de jour du jour en cours
		syslog(LOG_DEBUG, "etat vr salon : %d", stEtatVR.etat_vr_salon);
		
		// on récupère l'état du volet du chm
		sprintf(charetatvrchm,"%.*s", (char) lengths[2], row[2]);
		//on converti en int
		stEtatVR.etat_vr_chm = atoi(charetatvrchm);
		// on affiche le type de jour du jour en cours
		syslog(LOG_DEBUG, "etat vr chm : %d", stEtatVR.etat_vr_chm);
				
		// on récupère l'état du volet du chjf
		sprintf(charetatvrchjf,"%.*s", (char) lengths[3], row[3]);
		//on converti en int
		stEtatVR.etat_vr_chjf = atoi(charetatvrchjf);
		// on affiche le type de jour du jour en cours
		syslog(LOG_DEBUG, "etat vr chjf : %d", stEtatVR.etat_vr_chjf);
		
	}else{
		// euh... je réfléchis
		syslog(LOG_ERR, "ERREUR : PAS DE CONSIGNE DE TEMPERATURE TROUVEE");
		stDonnees.consigne_temperat = 0; 
	}

  	// on libère la requête
	mysql_free_result(result);
  
	fflush(stdout);
	return(0);
}

//----------------------------------------------------------------------
//!\brief           récupération des infos du jour dans la table calendrier
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int recup_info_jour(void){

	// Preparation de la requete MySQL
	// on ne récupère que les infos du jour en cours
	sprintf(query, "SELECT * FROM `calendrier` WHERE `date` >= DATE_FORMAT( NOW( ) , '%%y-%%m-%%d' ) LIMIT 0,1");

	// envoi de la requete
	syslog(LOG_DEBUG, "Envoi de la requete : %s", query);
	if (mysql_query(conn, query)) {
		// si la requete echoue on retourne 1
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
	}

	// la requête s'est bien passée, on rend le jeu de résultats disponible via le pointeur result
	syslog(LOG_DEBUG, "Recuperation des donnees");
	result = mysql_use_result(conn);

	// on récupère le nombre de champs
	// unsigned int num_champs = mysql_num_fields(result);
	// printf("Recuperation du nombre de champs de la table: %d\n",num_champs);

	// on récupère le résultat (une seule ligne en principe... si la requête est bien faite)
	if(row = mysql_fetch_row(result)){
		// on stocke la taille  dans le pointeur
		lengths = mysql_fetch_lengths(result);

		// on récupère la saison du jour en cours
		sprintf(saison,"%.*s", (char) lengths[row_saison], row[row_saison]);
		//saison = row[row_saison];
		// on affiche cette date (au format UNIX) pour la comparaison future avec la date courante
		syslog(LOG_DEBUG, "saison : %s", saison);

		// on récupère le type de jour  du jour en cours
		sprintf(type,"%.*s", (char) lengths[row_type_jour], row[row_type_jour]);
		//type = row[row_type_jour];
		// on affiche le type de jour du jour en cours
		syslog(LOG_DEBUG, "type_jour : %s", type);
		
		// on libère la requête
		mysql_free_result(result);
		
		return(0);
	}
	else{
		// euh... je réfléchis
		syslog(LOG_DEBUG, "ERREUR : VERIFIER LA VALIDITE DU CALENDRIER");
		
		// on libère la requête
		mysql_free_result(result);
		
		return(1);
	}
	return(0);
}

//----------------------------------------------------------------------
//!\brief           récupération de l'état des volets roulants
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int read_etat_vr(){

	// Preparation de la requete MySQL
	// on ne récupère que les infos du jour en cours
	sprintf(query, "SELECT * FROM `voletroulant_statut`");

	// envoi de la requete
	syslog(LOG_DEBUG, "Envoi de la requete : %s", query);
	if (mysql_query(conn, query)) {
		// si la requete echoue on retourne 1
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
	}

	// la requête s'est bien passée, on rend le jeu de résultats disponible via le pointeur result
	syslog(LOG_DEBUG, "Recuperation des donnees");
	result = mysql_use_result(conn);

	// on récupère le nombre de champs
	// unsigned int num_champs = mysql_num_fields(result);
	// printf("Recuperation du nombre de champs de la table: %d\n",num_champs);

	// on récupère le résultat (une seule ligne en principe... si la requête est bien faite)
	row = mysql_fetch_row(result);

	// on stocke la taille  dans le pointeur
	lengths = mysql_fetch_lengths(result);

	//déclaration de variables temporaires en char pour récupérer les données mysql
	char charmode[]="0", charetatvrbureau[]="0", charetatvrsalon[]="0", charetatvrchm[]="0", charetatvrchjf[]="0";

	// on récupère le mode de gestion des VR
	sprintf(charmode,"%.*s", (char) lengths[row_mode_VR], row[row_mode_VR]);
	//on converti en int
	stEtatVR.mode = atoi(charmode);
	// on affiche cette date (au format UNIX) pour la comparaison future avec la date courante
	syslog(LOG_DEBUG, "mode : %d", stEtatVR.mode);

	// on récupère l'état du volet du bureau
	sprintf(charetatvrbureau,"%.*s", (char) lengths[row_etat_bureau], row[row_etat_bureau]);
	//on converti en int
	stEtatVR.etat_vr_bureau = atoi(charetatvrbureau);
	// on affiche le type de jour du jour en cours
	syslog(LOG_DEBUG, "etat vr bureau : %d", stEtatVR.etat_vr_bureau);

	// on récupère l'état du volet de la chambre salon
	sprintf(charetatvrsalon,"%.*s", (char) lengths[row_etat_salon], row[row_etat_salon]);
	//on converti en int
	stEtatVR.etat_vr_salon = atoi(charetatvrsalon);
	// on affiche le type de jour du jour en cours
	syslog(LOG_DEBUG, "etat vr salon : %d", stEtatVR.etat_vr_salon);

	// on récupère l'état du volet chm
	sprintf(charetatvrchm,"%.*s", (char) lengths[row_etat_chm], row[row_etat_chm]);
	//on converti en int
	stEtatVR.etat_vr_chm = atoi(charetatvrchm);
	// on affiche le type de jour du jour en cours
	syslog(LOG_DEBUG, "etat vr chm : %d", stEtatVR.etat_vr_chm);

	// on récupère l'état du volet chjf
	sprintf(charetatvrchjf,"%.*s", (char) lengths[row_etat_chjf], row[row_etat_chjf]);
	  //on converti en int
	stEtatVR.etat_vr_chjf = atoi(charetatvrchjf);
	// on affiche le type de jour du jour en cours
	syslog(LOG_DEBUG, "etat vr chjf : %d", stEtatVR.etat_vr_chjf);

	// on libère la requête
	mysql_free_result(result);
	return(0);
}

//----------------------------------------------------------------------
//!\brief           jour ou nuit?
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int ephemerides(void){

	// Preparation de la requete MySQL
	// on ne récupère que les d'heure de lever/coucher du soleil
	sprintf(query, "SELECT UNIX_TIMESTAMP(addtime(DATE_FORMAT(MAKEDATE(YEAR(NOW()),DAYOFYEAR(NOW())),'%%Y-%%m-%%d %%H:%%i:%%s') , `lever`)) AS hlever_unix, UNIX_TIMESTAMP(addtime(DATE_FORMAT(MAKEDATE(YEAR(NOW()),DAYOFYEAR(NOW())),'%%Y-%%m-%%d %%H:%%i:%%s') , `coucher`)) AS hcoucher_unix FROM `ephemerides` WHERE DATE_FORMAT( `mois-jour` , '%%m-%%d' ) = DATE_FORMAT( NOW( ) , '%%m-%%d' ) LIMIT 1");

	// envoi de la requete
	syslog(LOG_DEBUG, "Envoi de la requete : %s", query);
	if (mysql_query(conn, query)) {
		// si la requete echoue on retourne 1
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
	}

	// la requête s'est bien passée, on rend le jeu de résultats disponible via le pointeur result
	//printf("\nRecuperation des donnees");
	result = mysql_use_result(conn);

	// on récupère le nombre de champs
	// unsigned int num_champs = mysql_num_fields(result);
	// printf("Recuperation du nombre de champs de la table: %d\n",num_champs);

	// on récupère le résultat (une seule ligne en principe... si la requête est bien faite)
	row = mysql_fetch_row(result);

	// on stocke la taille  dans le pointeur
	lengths = mysql_fetch_lengths(result);

	// on récupère l'heure de lever du soleil au format unixtime
	stDonnees.h_lever = atoi(row[0]);
	// on affiche cette date (au format UNIX) pour la comparaison future avec la date courante
	syslog(LOG_DEBUG, "heure lever (unixtime) : %d", stDonnees.h_lever);

	// on récupère l'heure de coucher du soleil au format unixtime
	stDonnees.h_coucher = atoi(row[1]);
	// on affiche cette date (au format UNIX) pour la comparaison future avec la date courante
	syslog(LOG_DEBUG, "heure coucher (unixtime) : %d", stDonnees.h_coucher);
		
  	// on libère la requête
	mysql_free_result(result);

	fflush(stdout);
	return(0);
}

//----------------------------------------------------------------------
//!\brief          Calcul le flux ...
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
float Calcul_fluxsolaire(void){

	//déclaration des variables
	char strtemp[10];
	float fPyraMoy = 0.0;

	// Preparation de la requete MySQL
	// requete pour récupérer les valeurs du pyranètre sur les 30 derniers minutes
	//SELECT ana1 FROM `pyranometre` WHERE date_time >= SUBTIME( now( ) , "00:30:00.0" ) 
	sprintf(query, "SELECT AVG(ana1)*454 FROM `pyranometre` WHERE date_time >= SUBTIME( now( ) , \"00:30:00.0\" ) ");

	// envoi de la requete
	// printf("\nEnvoi de la requete : %s", query);
	if (mysql_query(conn, query)) {
		// si la requete echoue on retourne 1
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
	}

	// la requête s'est bien passée, on rend le jeu de résultats disponible via le pointeur result
	//printf("\nRecuperation des donnees");
	result = mysql_store_result(conn);

	//si aucune valeur retournée
	// si la requête ne retourne rien (arrive à la première requete aprés la création de la table... ou si la sonde est HS)
	// on récupère le résultat 
	syslog(LOG_DEBUG, "Retour requet calcul flux solaire , mysql_num_rows(result) = %d", mysql_num_rows(result));
	if(mysql_num_rows(result) != 0){
		row = mysql_fetch_row(result);
		//On récupère le nombre de champs
		// num_champs = mysql_num_fields(result);
		// printf("Recuperation du nombre de champs de la table: %d\n",num_champs);
		
		//On stocke la taille  dans le pointeur
		lengths = mysql_fetch_lengths(result);
		
		// on récupère la temperature ext
		sprintf(strtemp,"%.*s", (char) lengths[0], row[0]);

		//conversion de la température (string) en entier
		fPyraMoy = atof(strtemp);
		
		//Affiche la valeur
		//printf ("temp_integer : %f \n", dbtemp);
	}
	else{
		// on a pas eu de valeur pyrano depuis 30 minutes !
		syslog(LOG_DEBUG, "Pas de valeur depuis 30 minutes !");	
		fPyraMoy = -1.0;
	}

	//on affiche la valeur calculée pour DEBUG
	syslog(LOG_DEBUG, "Valeur moyenne du flux solaire : %0.2f", fPyraMoy);	
	return (fPyraMoy);
}

//----------------------------------------------------------------------
//!\brief          Prépare un msg pour les volets roulants (table pile msg_radio_tx))
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int envoie_msg_etat_vr(int vr_bureau, int vr_salon, int vr_chm, int vr_chjf){

	// Preparation de la requete MySQL
	// on place le message à emettre dans la table tx_msg_radio
	sprintf(query, "INSERT INTO `domotique`.`tx_msg_radio` (`id` ,`date_time` ,`message` )VALUES (NULL , NOW(), '$VRL,%d,%d,%d,%d');",vr_bureau, vr_salon, vr_chm, vr_chjf);

	// envoi de la requete
	// printf("\nEnvoi de la requete : %s", query);
	if (mysql_query(conn, query)) {
		// si la requete echoue on retourne 1
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
	}

	syslog(LOG_DEBUG, "Message envoye au volet : $VRL,%d,%d,%d,%d",vr_bureau, vr_salon, vr_chm, vr_chjf);	

	// printf("\nMessage ecrit dans la pile");
	// s'il n'y a pas d'erreurs, tout est ok, on le dit
	return(0);
}

//----------------------------------------------------------------------
//!\brief          Ecrit dans la table etat_VR l'état des VR calculé (mode automatique)
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int ecrire_etat_vr_table(int vr_bureau, int vr_salon, int vr_chm, int vr_chjf){
	
	// Preparation de la requete MySQL
	// on place le message à emettre dans la table tx_msg_radio
	sprintf(query, "UPDATE `domotique`.`voletroulant_statut` SET `bureau` = '%d',`salon` = '%d',`chambreM` = '%d',`chambreJF` = '%d' WHERE `voletroulant_statut`.`id` =1;",vr_bureau, vr_salon, vr_chm, vr_chjf);

	// envoi de la requete
	// printf("\nEnvoi de la requete : %s", query);
	if (mysql_query(conn, query)) {
		// si la requete echoue on retourne 1
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
	}

	syslog(LOG_DEBUG, "Donnees stockees dans la table : $VRL,%d,%d,%d,%d",vr_bureau, vr_salon, vr_chm, vr_chjf);	
	
	// DEBUG
	// on place le message à emettre dans la table tx_msg_radio
	sprintf(query, "INSERT INTO `domotique`.`voletroulant_log` (`id`, `date_time`, `bureau`, `salon`, `chambreM`, `chambreJF`) VALUES (NULL, NOW(), '%d', '%d', '%d', '%d');",vr_bureau, vr_salon, vr_chm, vr_chjf);

	// envoi de la requete
	// printf("\nEnvoi de la requete : %s", query);
	if (mysql_query(conn, query)) {
		// si la requete echoue on retourne 1
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
	}
	
	// printf("\nMessage ecrit dans la pile");
	// s'il n'y a pas d'erreurs, tout est ok, on le dit
	return(0);
}

//***********************************************
//----------------------------------------------------------------------
//!\brief           le main !
//----------------------------------------------------------------------
//***********************************************
int main(int argc, char *argv[]) {  
  
  //***************************
  //* INITIALISATION
  //***************************
  printf("Installing signal handler...");
  (void) signal(SIGINT, sigfun);
  printf("done\n");

	openlog(log_name, LOG_CONS, LOG_USER);
	syslog(LOG_DEBUG, "***************************************************************");
	syslog(LOG_DEBUG, "Demarrage GESTION_VR.C, logiciel de gestion des volets roulants");
	printf("Pour les messages de DEBUG, voir syslog : \\var\\log\\user.log \n");
   
  //initialisation BDD
  if(init_mysql() == 1) {
    // erreur de liaison avec la bdd
    syslog(LOG_ERR, "Erreur de connexion a MySQL");
    exit(1);
  }
  
  //***************************************************************************
  //* PROGRAMME PRINCIPAL 
  //***************************************************************************

	// lecture de la date/heure courante
	// on récupère l'heure courante au format UNIX -> nb de secondes depuis le 01/01/1970 , je fais comme ça car c'est plus facile que de comparer des dates/heure
	// seconds = nb de secondes depuis le 1er janvier 1970
	// conversion en heure locale
	struct tm *local;
	stDonnees.seconds = time (NULL); 
	time_t t;
	t = time(NULL);
	// conversion en heure locale
	local = localtime(&t);
	syslog(LOG_DEBUG, "Heure et date courante : %s", asctime(local));
	syslog(LOG_DEBUG, "Unixtime : %d", t);
	syslog(LOG_DEBUG, " 0 -> VOLET_FERME , 1 -> VOLET_FERME , 2 -> VOLET_MIOMBRE , 3 -> IMMOBILE");

  
	// on lit l'état des VR et le mode en cours (1 seule ligne)
	if(read_etat_vr() == 1){
		// erreur sur la recuperation des messages
		syslog(LOG_ERR, "Erreur lors de la lecture de la position des VR depuis la bdd");
		exit(1);
	}	
	
	//si on est en mode AUTO
	if( stEtatVR.mode == AUTOMATIQUE){		
		
		// récupération des infos du jour dans la table planning
		if(recup_info_jour() == 1) {
			// erreur sur la recuperation des messages
			syslog(LOG_ERR, "Erreur lors de la recuperation des infos du jour");
			exit(1);
		}
		
		// récupération de la température de consigne
		if(recup_temperature(saison) == 1) {
			// erreur sur la recuperation des messages
			syslog(LOG_ERR, "Erreur lors de la recuperation de la temperature de saison");
			exit(1);
		}	

		
		// GESTION DU JOUR / NUIT
		// récupération de l'état jour/nuit
		if(ephemerides() == 1) {
			// erreur sur la recuperation des messages
			syslog(LOG_ERR, "Erreur lors de la lecture des ephemerides depuis la bdd");
			exit(1);
		}
		
		//on regarde si il fait jour ou nuit
		if(stDonnees.seconds >= stDonnees.h_lever && stDonnees.seconds < stDonnees.h_coucher)
			//il fait jour
			stDonnees.flag_jour = 1;
		else
			// il fait nuit
			stDonnees.flag_jour = 0;

		// FLUX SOLAIRE
		flux_solaire = Calcul_fluxsolaire();
		
		//si on a calculé un flux solaire cohérent
		if( flux_solaire > 0.0){
			//si on est en hiver
			if(!strcmp(saison, "H") || !strcmp(saison, "A")){
				// on regarde si le flux actuel est > au mini demandé en hiver
				if(flux_solaire > stDonnees.flux_solaire_trig)
					stDonnees.flag_flux = 1;
				else
					stDonnees.flag_flux = 0;
			}//si on est en ETE
			else if(!strcmp(saison, "E")){
				// on regarde si le flux actuel est < au maxi demandé en été
				if(flux_solaire < stDonnees.flux_solaire_trig)
					stDonnees.flag_flux = 1;
				else
					stDonnees.flag_flux = 0;
			}
		}
		else{
			// on force l'état en 'immobile'
			stEtatVR.etat_vr_bureau = stEtatVR.etat_vr_salon = stEtatVR.etat_vr_chm = stEtatVR.etat_vr_chjf = 3;
			syslog(LOG_DEBUG, "Etat des VR forcé à immobile (3)");
		}
		// on passe les conditions dans la table de vérité afin de récupérer l'état des VR
		etat_vr_mode_auto();
		//on sauvegarde les états dans la base de données
		ecrire_etat_vr_table(stEtatVR.etat_vr_bureau, stEtatVR.etat_vr_salon, stEtatVR.etat_vr_chm, stEtatVR.etat_vr_chjf);		
	}
	
	//on écrit un message dans la table rx_message_radio avec l'état des volets
	if(envoie_msg_etat_vr(stEtatVR.etat_vr_bureau, stEtatVR.etat_vr_salon, stEtatVR.etat_vr_chm, stEtatVR.etat_vr_chjf) == 1){
		// erreur sur la recuperation des messages
		syslog(LOG_ERR, "Erreur lors de l'envoie du message pour piloter les VR");
		exit(1);
	}


  // fermeture de MySQL
  mysql_close(conn);
  syslog(LOG_DEBUG, "Exit normally");
  closelog();
  printf("\nEXIT \n\n");

  //exit(0);
}
