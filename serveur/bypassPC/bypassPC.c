// Simple C program that connects to MySQL Database server
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <syslog.h>
//#define WITHMYSQL
#ifdef WITHMYSQL
#include <mysql/mysql.h>
#endif
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h> //gestion des repertoire
#include <signal.h>
#include <time.h>

#include <string.h>
#include <stddef.h>
#include "passwd.h"


#define FALSE 0
#define TRUE 1

#define SAISON_NULL 0
#define HIVER 1
#define ETE 2 
#define AUTO 3

//table calendrier
#define ROW_ID 0
#define ROW_DATE 1
#define ROW_SAISON 2
#define ROW_TYPE_JOUR 3

//table calendrier_saison
#define ROW_CONSIGNE_TEMPERATURE 2
#define ROW_TEMPERATURE 0

#define POSITION_PC 0
#define POSITION_EXT 100


// Variables
int keep_looping = TRUE;

char temp_ext_char[11];
double ftemp_ext;
char temp_pc_char[11];
double ftemp_pc;
char inf_saison[] ="A";  // saison "A" par défaut -> automatique
char inf_type_jour[] = "T"; // type de jour par défaut "T" = travail ... bah oué
float fconsigne_temperature = 19.0; //consigne de température, 19°C par défaut.

char msg_radio[66] = "";



//On déclare un pointeur long non signé pour y stocker la taille des valeurs
unsigned long *lengths;

int message_pointer = 0;
int bMessage_recu = FALSE;
unsigned int type_message = 0;
//pour la gestion de l'heure/date
time_t t;
struct tm *local;
char log_name[] = "bypass_PC";

//Déclaration du pointeur de structure de type MYSQL
MYSQL *conn;
MYSQL_RES *result = NULL;
MYSQL_ROW row = NULL;

unsigned int i = 0, j=0;
unsigned int num_champs = 0;


//prototypes
void sigfun(int sig);
int init_mysql(void);
float lire_temp_table(char *table);
int recup_temperatures(void);
unsigned int calcul_consigne_bypass(char *saison, float ftempconsigne);
int read_info_saison(char *inf_saison);
int recup_info_jour(char *table);
void make_message(unsigned int consignebypass);
int envoie_msg_radio(char *msg_radio);
int log_donnees(unsigned int consignebypass);


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
//!\brief           récupère la température (paramétrable) moyenne sur les 30 dernière minutes
//!\return        float : température demandée...
//----------------------------------------------------------------------
float lire_temp_table(char *table){

	//char strtemp[255];

	sprintf(query, "SELECT AVG( ana1 ) AS TempMoy FROM  `%s` WHERE date_time >= SUBTIME( NOW( ) ,  '00:30:00' )", table);

	//Envoi de la requete
	//printf("Envoi de la requete : %s\n", query);
	if (mysql_query(conn, query)) {
		// si la requete echoue on retourne le message d'erreur
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
	}
	
	//On met le jeu de résultat dans le pointeur result
	result = mysql_use_result(conn);
	
	float ftemp;
	
	while ((row = mysql_fetch_row(result))){
		//On récupère le nombre de champs
		// num_champs = mysql_num_fields(result);
		// printf("Recuperation du nombre de champs de la table: %d\n",num_champs);
		
		//On stocke la taille  dans le pointeur
		//lengths = mysql_fetch_lengths(result);
		
		// on récupère la temperature ext
		//sprintf(strtemp,"%.*s", (int) lengths[ROW_TEMPERATURE], row[ROW_TEMPERATURE]);
		
		//conversion de la température (string) en entier
		ftemp = atof(row[ROW_TEMPERATURE]);
		
		//Affiche la valeur
		//printf ("temp_integer : %f \n", dbtemp);
	}
	
	//Libération du jeu de résultat
	mysql_free_result(result);
	
	return(ftemp);
}
//----------------------------------------------------------------------
//!\brief           Récupère les températures dans la bdd
//!\return        -
//----------------------------------------------------------------------
int recup_temperatures(void){

	// ------- TEMPERATURE EXTERIEURE
	// Preparation de la requete
	// on ne récupère la valeur moyenne de la température des 15 dernières minutes
	char *table_capteur = "analog1";
	ftemp_ext = lire_temp_table(table_capteur);
	// on affiche cette température 
	syslog(LOG_DEBUG, "temp_ext_char : %f", ftemp_ext);
	
	// ------- TEMPERATURE PC
	// Preparation de la requete
	// on ne récupère la valeur moyenne de la température des 15 dernières minutes
	table_capteur = "analog3";
	ftemp_pc = lire_temp_table(table_capteur);
	// on affiche cette température 
	syslog(LOG_DEBUG, "temp_pc_char : %f", ftemp_pc);
		
	fflush(stdout);
}

//----------------------------------------------------------------------
//!\brief           calcul la position du bypass à partir du mode et de la température de consigne
//!\return        valeur de la consigne
//----------------------------------------------------------------------
unsigned int calcul_consigne_bypass(char *saison, float ftempconsigne){

	unsigned int consigne;
	
	//on converti le string en int pour faire le switch case derrière... pas super clean mais bon...
	int int_saison;
	if(!strcmp(saison, "H"))
		int_saison = HIVER;
	else if (!strcmp(saison, "A"))
		int_saison = AUTO;
	else if (!strcmp(saison, "E"))
		int_saison = ETE;
	else
		int_saison = SAISON_NULL;
		
		
	// Pour info
	syslog(LOG_DEBUG, "000 -> bypass donne l'air du puits canadien.");
	syslog(LOG_DEBUG, "100 -> bypass donne l'air extérieur.");

	switch(int_saison){
		// on est dans aucun des modes (au cas ou ça merderai...)
		case SAISON_NULL :
			syslog(LOG_DEBUG,"Erreur : mode inconnu, touche à ton c... ! (desole)");
			//par défaut on prend la position du PC, c'est là ou la température est la plus tempérée...
			consigne = POSITION_PC;
			break;
		
		case HIVER:
			// on cherche a faire entrer dans la VMC la temperature la plus chaude (no limit)
			if(ftemp_pc > ftemp_ext)
				consigne = POSITION_PC;
			else
				consigne = POSITION_EXT;
			break;
			
		case ETE:
			// on cherche a faire entrer dans la VMC la temperature la plus froide (no limit)
			if(ftemp_pc > ftemp_ext)
				consigne = POSITION_EXT;
			else
				consigne = POSITION_PC;
			break;
			
		case AUTO:
			// on cherche a faire entrer dans la VMC la temperature la plus proche de la temperature du consigne
			if(abs(ftemp_pc-ftempconsigne) > abs(ftemp_ext-ftempconsigne))
				consigne = POSITION_EXT;
			else
				consigne = POSITION_PC;
			break;
	}
	
	//on affiche la consigne calculee
	syslog(LOG_DEBUG,"Consigne : %d",consigne);
	
	return(consigne);
}

//----------------------------------------------------------------------
//!\brief           récupération des infos du jour dans la table calendrier
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int recup_info_jour(char *table){

  // Preparation de la requete MySQL
  // on ne récupère que les infos du jour en cours
  sprintf(query, "SELECT * FROM `%s` WHERE `date` = DATE_FORMAT( NOW( ) , '%%y-%%m-%%d' )", table);

  // envoi de la requete
  syslog(LOG_DEBUG,"Envoi de la requete : %s", query);
  if (mysql_query(conn, query)) {
    // si la requete echoue on retourne 1
    fprintf(stderr, "%s\n", mysql_error(conn));
    return(1);
  }

  // la requête s'est bien passée, on rend le jeu de résultats disponible via le pointeur result
  syslog(LOG_DEBUG,"Recuperation des donnees");
  result = mysql_use_result(conn);

  // on récupère le nombre de champs
  // unsigned int num_champs = mysql_num_fields(result);
  // printf("Recuperation du nombre de champs de la table: %d\n",num_champs);
  
  // on récupère le résultat (une seule ligne en principe... si la requête est bien faite)
  row = mysql_fetch_row(result);
  
  // on stocke la taille  dans le pointeur
  lengths = mysql_fetch_lengths(result);
  
  // on libère la requête
  mysql_free_result(result);
    
  fflush(stdout);
  return(0);
}

//----------------------------------------------------------------------
//!\brief           lit la date du message en cours et convertit en timestamp
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int read_info_jour(void){
  
	// on récupère la saison du jour en cours
	sprintf(inf_saison,"%.*s", (char) lengths[ROW_SAISON], row[ROW_SAISON]);

	// on affiche cette date (au format UNIX) pour la comparaison future avec la date courante
	syslog(LOG_DEBUG,"inf_saison : %s", inf_saison);

	// on récupère le type de jour  du jour en cours
	sprintf(inf_type_jour,"%.*s", (char) lengths[ROW_TYPE_JOUR], row[ROW_TYPE_JOUR]);

	// on affiche le type de jour du jour en cours
	syslog(LOG_DEBUG,"inf_type_jour : %s", inf_type_jour);

	fflush(stdout);
	return(0);
}


//----------------------------------------------------------------------
//!\brief          Dans la table saison, faire une requête pour récupérer les infos de températures en fonction de la saison
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int read_info_saison(char *inf_saison){
	// Preparation de la requete MySQL
	// on ne récupère que les infos du jour en cours
	sprintf(query, "SELECT * 	FROM `calendrier_saison` WHERE `type` = '%s'", inf_saison);

	// envoi de la requete
	syslog(LOG_DEBUG,"Envoi de la requete : %s", query);
	if (mysql_query(conn, query)) {
	// si la requete echoue on retourne 1
	fprintf(stderr, "%s\n", mysql_error(conn));
	return(1);
	}

	// la requête s'est bien passée, on rend le jeu de résultats disponible via le pointeur result
	result = mysql_use_result(conn);

	// on récupère le résultat (une seule ligne en principe... si la requête est bien faite)
	if (row = mysql_fetch_row(result)){
	  // on récupère la consigne de température
	   fconsigne_temperature = atof(row[ROW_CONSIGNE_TEMPERATURE]);
	  // on affiche la valeur de la consigne de température pour le type de saison récupéré
	  syslog(LOG_DEBUG,"consigne température : %.1f ", fconsigne_temperature);
	}
	// on libère la requête
	mysql_free_result(result);

	fflush(stdout);
	return(0);
}


//----------------------------------------------------------------------
//!\brief           Fabrique le message en fonction de la consigne du bypass
//!\return        -
//----------------------------------------------------------------------
void make_message(unsigned int consignebypass){

	//on crée le message a envoyer
	sprintf(msg_radio,"$BPC,%d,0", consignebypass);				

	//on affiche le message cree
	syslog(LOG_DEBUG,"message radio : %s",msg_radio);	
}

//----------------------------------------------------------------------
//!\brief           Copie du message dans la BDD
//!\return        taille du message envoyé, 0 si erreur
//----------------------------------------------------------------------
int envoie_msg_radio(char *msg_radio){

	// Preparation de la requete MySQL
	sprintf(query, "INSERT INTO %s VALUES('',NOW(), '%s')", "tx_msg_radio", msg_radio);
	fflush(stdout);

	if (mysql_query(conn, query)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
    }

	fflush(stdout);
	return(0);
}

//----------------------------------------------------------------------
//!\brief           Fonction qui log la structure stDonnees dans la BDD pour mise au point
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int log_donnees(unsigned int consignebypass){

	// Preparation de la requete MySQL
	// on ne stocke que les données de la structure
	sprintf(query, "INSERT INTO `domotique`.`bypass_pc_log` (`id`, `date_time`, `consigne`) VALUES (NULL, NOW(), '%d');", consignebypass);	// envoi de la requete
	//printf("\nEnvoi de la requete : %s", query);
	if (mysql_query(conn, query)) {
		// si la requete echoue on retourne 1
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
	}

	syslog(LOG_DEBUG,"Message de log ecrit dans la bdd");	
	// s'il n'y a pas d'erreurs, tout est ok, on le dit
	
	// flush pour afficher tout les messages et debug
	fflush(stdout);
	return(0);
}


//----------------------------------------------------------------------
//!\brief           ouverture d'une connexion à la bdd
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int init_mysql(void){

	syslog(LOG_DEBUG,"With MySQL support");
	//Initialisation liaison avec mysql
	conn = mysql_init(NULL);

	// Connexion à la BDD
	syslog(LOG_DEBUG,"Connexion a la BDD");	
	if(!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
	}

	// Envoi de la requete de sélection de la base à utiliser
	//printf("Envoi de la requete USE DOMOTIQUE\n");
	if (mysql_query(conn, "USE domotique")) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
	}
	// s'il n'y a pas d'erreurs, tout est ok, on le dit
	return(0);
}

//----------------------------------------------------------------------
//!\brief           le main !
//----------------------------------------------------------------------
int main(void) {  
  
	//***************************
	//* INITIALISATION
	//***************************
	printf("Installing signal handler...");
	(void) signal(SIGINT, sigfun);
	printf("done\n");

	openlog(log_name, LOG_CONS, LOG_USER);
	syslog(LOG_DEBUG, "***************************************************************");
	syslog(LOG_DEBUG, "Demarrage bypassPC.c, logiciel de gestion du bypass du puits canadien");
	printf("Pour les messages de DEBUG, voir syslog : \\var\\log\\user.log \n");

	//lecture de la date/heure courante
	t = time(NULL);
	// conversion en heure locale
	local = localtime(&t); 
	syslog(LOG_DEBUG, "Heure et date courante : %s", asctime(local));
	syslog(LOG_DEBUG, "Unixtime : %d", t);

	//***************************************************************************
	//* PROGRAMME PRINCIPAL 
	//***************************************************************************

	//initialisation BDD
	if(init_mysql() == 1) {
		// erreur de liaison avec la bdd
		// prévoir un msg de log vers un fichier de log
		exit(1);
	}

	// Récupération des données de températures : Ext, Int et PC
	if(recup_temperatures() == 1){
		// erreur sur la recuperation des messages
		// prévoir un msg de log vers un fichier de log
		exit(1);
	}

	// récupération des infos du jour dans la table planning
	if(recup_info_jour("calendrier") == 1) {
		// erreur sur la recuperation des messages
		// prévoir un msg de log vers un fichier de log
		exit(1);
	}

	// on lit les infos du jour dans le résultat de la reqête (1 seule ligne)
	if(read_info_jour() == 1){
		// erreur sur la recuperation des messages
		// prévoir un msg de log vers un fichier de log
		exit(1);
	}

	//Récupération du type de consigne : Hiver  / Ete / Auto et de la température de consigne
	// on récupére les infos de températures en fonction de la saison
	if(read_info_saison(inf_saison) == 1){
		// erreur sur la recuperation des messages
		// prévoir un msg de log vers un fichier de log
		exit(1);
	}

	int consignebypass = calcul_consigne_bypass(inf_saison, fconsigne_temperature);
	
	// Préparation du message à envoyer
	make_message(consignebypass);
	
	// Copie du message dans la BDD
	if(envoie_msg_radio(msg_radio) == 1){
		// erreur sur la recuperation des messages
		// prévoir un msg de log vers un fichier de log
		exit(1);
	}
	
	//avant de sortir, on log les données de la structure stDonnees pour la mise au pointeur
	if(log_donnees(consignebypass) == 1) {
		// erreur dans le log
		// prévoir un msg de log vers un fichier de log
		exit(1);
	}

	//Fermeture de MySQL
	mysql_close(conn);

	syslog(LOG_DEBUG, "Exit normally");
	closelog();

	return 0;
}
