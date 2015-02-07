//--------------------------------------------------
//! \file     receiver.c
//! \brief    Simple C program that connects to MySQL Database server
//! \date     05-11-2014
//! \author   minbiocabanon
//--------------------------------------------------

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
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
#include <string.h>
#include <stddef.h>
#include <syslog.h>

#include "passwd.h"

#define BAUDRATE B57600
#define _POSIX_SOURCE 1 // POSIX compliant source
#define FALSE 0
#define TRUE 1
#define TAILLE_MESSAGE 255

// #define FILE_TELEINFO "/var/data/domotique/teleinfo/teleinfo.dat"
// #define FILE_TEMPERAT "/var/data/domotique/analog1/temperature.dat"
// #define FILE_GARBAGE  "/var/data/domotique/garbage/data_garbage.dat"

// définition pour le tracing des erreurs
typedef enum TRACE TRACE;
enum TRACE{
	C_OK,
	C_ENTETE,
	C_HC,
	C_HP,
	C_TARIF,
	C_COURANT,
	C_COURANTMAX,
	C_PUISSANCE,
	C_ENTETE2,
	C_ANA1,
	C_ANA2,
	C_ANA3,
	C_ANA4,
	C_ENTETE_PYR,
	C_ANA1_PYR,
	C_ENTETE_POELE,
	C_TYPE_POELE,
	C_POELE_MOD,
	C_POELE_SAC,
	C_POELE_NVG,
	C_TYPE_TND
	};

typedef enum MSG_RADIO MSG_RADIO;
enum MSG_RADIO{
	MSG_NULL, 		// 0
	MSG_TELEINFO,	// 1
	MSG_ANALOG1,	// 2
	MSG_ANALOG2,	// 3
	MSG_ANALOG3,	// 4
	MSG_ANALOG4,	// 5
	MSG_ANALOG5,	// 6
	MSG_ANALOG6,	// 7
	MSG_PYR,		// 8
	MSG_POELE,		// 9
	MSG_VMC,		// 10
	MSG_TND			// 11
	};

// prototypes des fonctions utilisées
int decoupe_message_ana(char *message);
int decoupe_message_teleinfo(char *message);
void sigfun(int sig);
int init_mysql(void);
int ConfigOpenPortCOM(int argc, char *argv[]);
int put_into_teleinfo_database(char *table, char *entete, int HC, int HP, char *tarif, int courant, int courantmax, int puissance);
int put_into_analog_database(char *table, char *entete, float ana1, float ana2, float ana3, float ana4);
int put_into_pyranometre_database(char *table, char *entete, float ana1);
int put_into_pellet_database(char *table, char *entete, int nb_sac);
int put_into_pelletres_database(char *table, char *entete, int niveau_res);
int put_into_tnd_database(char *table, char *entete, long flat, long flon);
int trt_msg_teleinfo(char *message, char *table);
int trt_msg_analog(char *message,char *table);
int trt_msg_pyranometre(char *message,char *table);
int trt_msg_poele(char *message, char *table);
int trt_msg_tnd(char *message, char *table);
int main(int argc, char *argv[]);

int keep_looping = TRUE;
unsigned int time_out_portCOM = 0;
int fd;
// mémorise le point d'erreur
int trace = C_OK;
struct termios oldtio,newtio;

//variables pour les messages	char *entete, *tarif;
int HC, 
	HP, 
	courant, 
	courantmax, 
	puissance;
char 	*entete, 
		*tarif;
int ana1, 
	ana2, 
	ana3, 
	ana4;
char buf[255];
int bMessage_recu = FALSE;
unsigned int type_message = 0;
int message_pointer = 0;
char message[TAILLE_MESSAGE] = "still empty";
int c, taille_msg;
char log_name[] = "receiver";
char szTrace[255];

int compteur_de_bug = 0 ;
	
// On déclare un pointeur long sur un non signé pour y stocker la taille des valeurs
unsigned long *lengths;

// message des traces
char aszTrace[20][20] = {"OK", "ENTETE", "HC", "HP", "TARIF", "COURANT", "COURANTMAX", "PUISSANCE",
                     "ENTETE2", "ANA1", "ANA2", "ANA3", "ANA4", "C_ENTETE_PYR", "C_ANA1_PYR", 
					 "C_ENTETE_POELE", "C_TYPE_POELE", "C_POELE_MOD", "C_POELE_SAC", "C_POELE_NVG"};

// pour la gestion de l'heure/date
time_t t;
struct tm *local;
unsigned int time_unix_msg;

// Déclaration du pointeur de structure de type MYSQL
MYSQL *conn;
MYSQL_RES *result = NULL;
MYSQL_ROW row = NULL;
FILE *file;


//----------------------------------------------------------------------
//!\brief     	blindage du strtok
//!\return   	-1 si erreur
//----------------------------------------------------------------------
int decoupe_message_teleinfo(char *message){
	char *tmpstr;
	if(strlen(message) != 44) {
		return -1;
	}
	if((entete = strtok (message, ",")) == NULL) {
		perror("strtok:");
		trace = C_ENTETE;
		return -1;
	}
	if((tmpstr = strtok (NULL, ",")) == NULL) { 
		perror("strtok:");
		trace = C_HC;
		return -1;
	}
	else {
		HC = atoi(tmpstr); 
	}
	if((tmpstr = strtok (NULL, ",")) == NULL) { 
		perror("strtok:");
		trace = C_HP;
		return -1;
	}
	else {
		HP = atoi(tmpstr); 
	}
	if((tmpstr = strtok (NULL, ",")) == NULL) { 
		perror("strtok:");
		trace = C_TARIF;
		return -1;
	}
	else {
		tarif = tmpstr ; 
	}
	if((tmpstr = strtok (NULL, ",")) == NULL) { 
		perror("strtok:");
		trace = C_COURANT;
		return -1;
	}
	else {
		courant = atoi(tmpstr); 
	}
	if((tmpstr = strtok (NULL, ",")) == NULL) { 
		perror("strtok:");
		trace = C_COURANTMAX;
		return -1;
	}
	else {
		courantmax = atoi(tmpstr); 
	}
	if((tmpstr = strtok (NULL, ",")) == NULL) { 
		perror("strtok:");
		trace = C_PUISSANCE;
		return -1;
	}
	else {
		puissance = atoi(tmpstr); 
	}
}


//----------------------------------------------------------------------
//!\brief   	blindage du strtok
//!\return    	-1 si erreur
//----------------------------------------------------------------------
int decoupe_message_ana(char *message){
	char *tmpstr;
	
	if((entete = strtok (message, ",")) == NULL) {
		perror("strtok:");
		trace = C_ENTETE2;
		return -1;
	}
	if((tmpstr = strtok (NULL, ",")) == NULL) { 
		perror("strtok:");
		trace = C_ANA1;
		return -1;
	}
	else {
		ana1 = atoi(tmpstr); 
	}
	if((tmpstr = strtok (NULL, ",")) == NULL) { 
		perror("strtok:");
		trace = C_ANA2;
		return -1;
	}
	else {
		ana2 = atoi(tmpstr); 
	}
	if((tmpstr = strtok (NULL, ",")) == NULL) { 
		perror("strtok:");
		trace = C_ANA3;
		return -1;
	}
	else {
		ana3 = atoi(tmpstr); 
	}
	if((tmpstr = strtok (NULL, ",")) == NULL) { 
		perror("strtok:");
		trace = C_ANA4;
		return -1;
	}
	else {
		ana4 = atoi(tmpstr); 
	}
}


//----------------------------------------------------------------------
//!\brief           trap du control C
//!\param[in]     	sig paramètre qui ne sert à rien
//----------------------------------------------------------------------
void sigfun(int sig) {
    printf("You have pressed Ctrl-C , aborting!\n");
    keep_looping = FALSE;
	sprintf(szTrace, "receiver : You have pressed Ctrl-C , aborting!\n");
	syslog(LOG_DEBUG, szTrace);	
    exit(0);
}


//----------------------------------------------------------------------
//!\brief           ouverture d'une connexion à la bdd
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int init_mysql(void){
  //Initialisation liaison avec mysql
  conn = mysql_init(NULL);
    
  // Connexion à la BDD
  // printf("Connexion a la BDD\n");
  if(!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) {
    fprintf(stderr, "%s\n", mysql_error(conn));
    return(1);
  }
  
  // Envoi de la requete de sélection de la base à utiliser
  // printf("Envoi de la requete USE DOMOTIQUE\n");
  if (mysql_query(conn, "USE domotique")) {
    fprintf(stderr, "%s\n", mysql_error(conn));
    return(1);
  }
  // s'il n'y a pas d'erreurs, tout est ok, on le dit
  return(0);
}


//----------------------------------------------------------------------
//!\brief           Fermeture connexion mysql
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int close_mysql(void){

	//printf("Deconnexion MySQL\n");
	// fermeture de MySQL
	mysql_close(conn);
	// s'il n'y a pas d'erreurs, tout est ok, on le dit
	return(0);
}

//----------------------------------------------------------------------
//!\brief           Réalise un set ou reset du DTR
//!\param[in]     tty_fd pointer sur le device
//!\param[in]     on : 1=SET , 0=RESET
//!\return        retour de la fonction ioctl
//----------------------------------------------------------------------
int setdtr (int tty_fd, int on)
{
  int controlbits = TIOCM_DTR;
  if(on)
    return(ioctl(tty_fd, TIOCMBIC, &controlbits));
  else
    return(ioctl(tty_fd, TIOCMBIS, &controlbits));
} 


//----------------------------------------------------------------------
//!\brief           ouverture et configuration du port com
//!\param[in]     argc nombre de paramètres reçus depuis le main()
//!\param[in]     argv liste des paramètres du main
//!\return        -1 si le nb de params est incorrect
//----------------------------------------------------------------------
int ConfigOpenPortCOM(int argc, char *argv[]){

  // printf("connecting to port %s\n", argv[1]);

  // // ouverture du port série
  // fd = open(argv[1], O_RDWR | O_NOCTTY );
  // // erreur d'ouverture
  // if (fd <0) {
    // perror(argv[1]); 
    // // on sort
    // exit(-1); 
  // }
  // // save current port settings 
  // tcgetattr(fd,&oldtio); 

  // bzero(&newtio, sizeof(newtio));
  // newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
  // newtio.c_iflag = IGNPAR;
  // newtio.c_oflag = 0;

  // // set input mode (non-canonical, no echo,...) 
  // newtio.c_lflag = 0;

  // newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
  // newtio.c_cc[VMIN]     = 1;   /* blocking read until 1 chars received */

  // tcflush(fd, TCIFLUSH);
  // // on rend effectif le paramétrage du port série
  // tcsetattr(fd,TCSANOW,&newtio);


  if(argc != 2) {
    printf("Incorrect number of args: usage arduinomonitor <USB port> e.g. /dev/ttyUSB0\n");
    return(-1);
  }
  printf("connecting to port %s\n", argv[1]);

	// ouverture du port série  , j'ai mis O_RDONLY pour lecture seule du portCOM... afin d'éviter les conflits avec receiver. com
	// l'option O_RDWR fait une ouverture en read/write
	fd = open(argv[1], O_RDONLY | O_NONBLOCK | O_NOCTTY );
	// erreur d'ouverture
	if (fd < 0) {
		perror(argv[1]); 
		// on sort
		return(-1); 
	}
	// save current port settings 
	if(tcgetattr(fd,&oldtio) < 0){
		perror ("tcgetattr");
		// on sort
		return(-1);
	}

	/* Configure la ligne RS 232 COM0 */
	cfsetospeed (&newtio, B57600); // 19200 Bits/seconde
	newtio.c_cflag &= ~CSIZE;
	newtio.c_cflag |= CS8; // 7 bits de Data
	newtio.c_cflag |= HUPCL; // 7 bits de Data
	newtio.c_cflag |= PARENB;
	newtio.c_cflag |= IGNPAR; // Pas de Parité
	newtio.c_cflag &= CSTOPB; // 1 stop bit
	
	// bzero(&newtio, sizeof(newtio));
	// newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
	// newtio.c_iflag = IGNPAR;
	// newtio.c_oflag = 0;

	// set input mode (non-canonical, no echo,...) 
	newtio.c_lflag = 0;

	// inter-character timer in 1/100s
	// returns after a 100ms blank
	newtio.c_cc[VTIME] = 10; 
	// newtio.c_cc[VTIME] = 0;
	// blocking read until 100 chars received
	newtio.c_cc[VMIN] = 100;
	// newtio.c_cc[VMIN] = 1;
	// So, return must occur when a continuous flow of VMIN bytes max
	// separated by at the most VTIME/100 s

	tcflush(fd, TCIFLUSH);
	// on rend effectif le paramétrage du port série
  
	if( tcsetattr(fd,TCSANOW,&newtio) < 0 ){
		perror ("tcsetattr");
		// on sort
		return(-1);
	}

	fprintf(stderr,"Clearing DTR\n");
	setdtr(fd,0);
	// Pause for one second
	sleep(1);
	fprintf(stderr,"Setting DTR\n");
	setdtr(fd,1);
  
	// tout s'est bien passé
	return(0);
}


//----------------------------------------------------------------------
//!\brief           fonction qui exécute la requete mysql pour ferme la connexion
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int exec_requete(void) {

	// on envoie la requete avec controle du retour
	if (mysql_query(conn, query)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
	}else
		return(0);

}


//----------------------------------------------------------------------
//!\brief           Ajoute dans la table teleinfo les donnees de teleinfo
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int put_into_teleinfo_database(char *table, char *entete, int HC, int HP, char *tarif, int courant, int courantmax, int puissance) {

	//initialisation BDD
	if(init_mysql() == 1) {
	// erreur de liaison avec la bdd
	sprintf(szTrace, "receiver : erreur a l'ouverture de la BDD");
	syslog(LOG_DEBUG, szTrace);
	closelog();
	exit(1);
	}
	else{
		// on prépare la requete
		sprintf(query, "INSERT INTO %s VALUES('',NOW(), '%s', %d, %d, '%s', %d, %d, %d,'')", table, entete, HC, HP, tarif, courant, courantmax, puissance);	
		// on envoie la requete avec controle du retour
		if (mysql_query(conn, query)) {
			fprintf(stderr, "%s\n", mysql_error(conn));
			return(1);
		}
		close_mysql();
	}

	// on retourne 0 car tout est ok
	return(0);
}


//----------------------------------------------------------------------
//!\brief           Ajoute dans la table passée en paramètre les infos des capteurs. Fonction pour les table/message de type ana'x'
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int put_into_analog_database(char *table, char *entete, float ana1, float ana2, float ana3, float ana4) {

	//initialisation BDD
	if(init_mysql() == 1) {
	// erreur de liaison avec la bdd
	sprintf(szTrace, "receiver : erreur a l'ouverture de la BDD");
	syslog(LOG_DEBUG, szTrace);
	closelog();
	exit(1);
	}
	else{
		// send SQL query 
		sprintf(query, "INSERT INTO %s VALUES('',NOW(), '%s', %2.2f, %2.2f, %2.2f, %2.2f,0)", table, entete, ana1/100, ana2/100, ana3/100, ana4/100);
		if (mysql_query(conn, query)) {
			fprintf(stderr, "%s\n", mysql_error(conn));
			return(1);
		}
		close_mysql();
	}
	// on retourne 0 car tout est ok
	return(0);
}


//----------------------------------------------------------------------
//!\brief           Ajoute dans la table adéquat les infos du pyranomètre
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int put_into_pyranometre_database(char *table, char *entete, float ana1) {

	//initialisation BDD
	if(init_mysql() == 1) {
	// erreur de liaison avec la bdd
	sprintf(szTrace, "receiver : erreur a l'ouverture de la BDD");
	syslog(LOG_DEBUG, szTrace);
	closelog();
	exit(1);
	}
	else{
		// send SQL query 
		sprintf(query, "INSERT INTO %s VALUES('',NOW(), '%s', %2.2f,0)", table, entete, ana1/100);
		if (mysql_query(conn, query)) {
			fprintf(stderr, "%s\n", mysql_error(conn));
			return(1);
		}
		close_mysql();
	}	
	// on retourne 0 car tout est ok
	return(0);
}


//----------------------------------------------------------------------
//!\brief           Ajoute dans la table adéquat le nb de sac mis dans le réservoir du poele
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int put_into_pellet_database(char *table, char *entete, int nb_sac){

	//initialisation BDD
	if(init_mysql() == 1) {
	// erreur de liaison avec la bdd
	sprintf(szTrace, "receiver : erreur a l'ouverture de la BDD");
	syslog(LOG_DEBUG, szTrace);
	closelog();
	exit(1);
	}
	else{
		int stock_pellet = 0;

		// On prépare la requete qui récupère le nb de sacs en granulés
		sprintf(query, "SELECT capital FROM pellets	ORDER BY id DESC LIMIT 0 , 1");
		
		if (mysql_query(conn, query)) {
			fprintf(stderr, "%s\n", mysql_error(conn));
			return(1);
		}
			
		result = mysql_use_result(conn);
		
		// si la requete a retourné une ligne non nulle
		// on lit la ligne demandée
		if (row = mysql_fetch_row(result)){
			//on converti la chaine en integer
			stock_pellet = atoi(row[0]);
			sprintf(szTrace, "receiver : Nb de sacs en stock: %d", stock_pellet);
			syslog(LOG_DEBUG, szTrace);
		}
		else {
			// euh... je réfléchis
			printf("\n  ERREUR : RECUPERATION STOCK DE GRANULES IMPOSSIBLE");
			stock_pellet = 0; 
		}
		
		// on libère la requête
		mysql_free_result(result);  

		// On ajoute une ligne pour mettre à jour le stock
		sprintf(query, "INSERT INTO %s VALUES('',NOW(), '0', %d, %d)", table, nb_sac, (stock_pellet - nb_sac));
		
		sprintf(szTrace, "receiver : requete pour les pellets -> %s", query);
		syslog(LOG_DEBUG, szTrace);	
		
		if (mysql_query(conn, query)) {
			fprintf(stderr, "%s\n", mysql_error(conn));
			return(1);
		}
		
		close_mysql();
	}
	// on retourne 0 car tout est ok
	return(0);
}

//----------------------------------------------------------------------
//!\brief           Ajoute dans la table adéquat le niveau de granule mesuré dans le réservoir du poele
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int put_into_pelletres_database(char *table, char *entete, int niveau_res){

	//initialisation BDD
	if(init_mysql() == 1) {
	// erreur de liaison avec la bdd
	sprintf(szTrace, "receiver : erreur a l'ouverture de la BDD");
	syslog(LOG_DEBUG, szTrace);
	closelog();
	exit(1);
	}
	else{
		// send SQL query 
		sprintf(query, "INSERT INTO %s VALUES('',NOW(), '%s', %3d,0)", table, entete, niveau_res);
		if (mysql_query(conn, query)) {
			fprintf(stderr, "%s\n", mysql_error(conn));
			return(1);
		}
		close_mysql();	
	}
	// on retourne 0 car tout est ok
	return(0);
}


//----------------------------------------------------------------------
//!\brief           Ajoute dans la table passée en paramètre les infos GPS envoyées par le robot tondeuse.
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int  put_into_tnd_database(char *table, char *entete, long flat, long flon){

	//initialisation BDD
	if(init_mysql() == 1) {
	// erreur de liaison avec la bdd
	sprintf(szTrace, "receiver : erreur a l'ouverture de la BDD");
	syslog(LOG_DEBUG, szTrace);
	closelog();
	exit(1);
	}
	else{
		float tmplat = (float)flat/1000000.0;
		float tmplon = (float)flon/1000000.0;
		
		sprintf(szTrace, "receiver : GPS tondeuse , tmpflat:%7.6f   tmpflon:%7.6f",tmplat, tmplon);
		syslog(LOG_DEBUG, szTrace);		
		
		// send SQL query 
		sprintf(query, "INSERT INTO %s VALUES('',NOW(), '%s', %f, %f)", table, entete, tmplat, tmplon);

		sprintf(szTrace, "receiver : requete pour les positions GPS de la tondeuse -> %s", query);
		syslog(LOG_DEBUG, szTrace);	 

		if (mysql_query(conn, query)) {
			fprintf(stderr, "%s\n", mysql_error(conn));
			return(1);
		}
		close_mysql();	
	}
	// on retourne 0 car tout est ok
	return(0);
}


//----------------------------------------------------------------------
//!\brief           dépouille les infos du message teleinfo $EDF
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int trt_msg_teleinfo(char *message, char *table){
	printf("\n  Traitement msg teleinfo");    
	//***************************
	//* Log dans la base de données
	//***************************

	if( decoupe_message_teleinfo(message) != -1 ){
		// sprintf(szTrace, "receiver : Entete:%s ",entete);
		// syslog(LOG_DEBUG, szTrace);	 	
		// sprintf(szTrace, "receiver : puissance:%d ",puissance);		
		// syslog(LOG_DEBUG, szTrace);	 	
		// sprintf(szTrace, "receiver : Entete:%s ,HC:%d, HP:%d, tarif:%s, courant:%d, courantmax:%d, puissance:%d",entete,HC,HP,tarif,courant,courantmax,puissance);
		// syslog(LOG_DEBUG, szTrace);	 	          
		put_into_teleinfo_database(table, entete, HC, HP, tarif, courant, courantmax, puissance);
		//DEBUG
		printf("  -> enreg. dans base de donnees\n");
	}
	else{
		//DEBUG
		compteur_de_bug++;
		printf("  Erreur dans le strtok");
	}
	
	// flush pour afficher tout les messages et debug
	fflush(stdout);

	// on retourne 0 car tout est ok
	return(0);
}


//----------------------------------------------------------------------
//!\brief           Dépouille les infos du message $AN'x'
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int trt_msg_analog(char *message,char *table){
	printf("\n  Traitement msg %s", table);

	//***************************
	//* Log dans la base de données
	//***************************

	if (decoupe_message_ana(message) != -1){
		// sprintf(szTrace, "receiver : Entete:%s ,ana1:%d, ana2:%d, ana3:%d, ana4:%d",entete,ana1,ana2,ana3,ana4);
		// syslog(LOG_DEBUG, szTrace);	 		         
		put_into_analog_database(table, entete, ana1, ana2, ana3, ana4);
		//DEBUG
		printf("  -> enreg. dans base de donnees\n");
		fflush(stdout);
	}
	else{
		//DEBUG
		compteur_de_bug++;
		printf("  Erreur dans le strtok");	
	}
	// on retourne 0 car tout est ok
	return(0);
}


//----------------------------------------------------------------------
//!\brief           Dépouille les infos du message $PYR
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int trt_msg_pyranometre(char *message, char *table){
	printf("\n  Traitement msg %s", table);
	  
	//***************************
	//* Log dans la base de données
	//***************************
	char *tmpstr;
	
	if((entete = strtok (message, ",")) == NULL) {
		perror("strtok:");
		compteur_de_bug++;
		trace = C_ENTETE_POELE;
		return -1;
	}
	if((tmpstr = strtok (NULL, ",")) == NULL) { 
		perror("strtok:");
		compteur_de_bug++;
		trace = C_TYPE_POELE;
		return -1;
	}
	else {
		ana1 = atoi(tmpstr); 
		// sprintf(szTrace, "receiver : Entete:%s ,ana1:%d",entete,ana1);
		// syslog(LOG_DEBUG, szTrace);	 		       
		put_into_pyranometre_database(table, entete, ana1);
		//DEBUG
		printf("  -> enreg. dans base de donnees\n");
		fflush(stdout);
		// on retourne 0 car tout est ok
		return(0);
	}
}


//----------------------------------------------------------------------
//!\brief           Dépouille les infos du message $POL
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int trt_msg_poele(char *message, char *table){
	printf("\n  Traitement msg %s", table);
	
	//***************************
	//* Log dans la base de données
	//***************************
	char *tmpstr;
	 
	if((entete = strtok (message, ",")) == NULL) {
		perror("strtok:");
		compteur_de_bug++;
		trace = C_ENTETE_PYR;
		return -1;
	}
	if((tmpstr = strtok (NULL, ",")) == NULL) { 
		perror("strtok:");
		compteur_de_bug++;
		trace = C_ANA1_PYR;
		return -1;
	}
	else{
		//si c'est un message de type changement de mode
		if(!strncmp(tmpstr, "MOD",3)){
			if((tmpstr = strtok (NULL, ",")) == NULL) { 
				perror("strtok:");
				compteur_de_bug++;
				trace = C_POELE_MOD;
				return -1;
			}			
			int mode_poele = atoi(tmpstr); 
			sprintf(szTrace, " receiver : msg poele MOD mode:%d", mode_poele);
			syslog(LOG_DEBUG, szTrace);	         
			//put_into_pyranometre_database(table, entete, ana1);
		}
		//si c'est un message qu indique le nombre de sac mis dans le réservoir du poele
		else if(!strncmp(tmpstr, "SAC",3)){
			if((tmpstr = strtok (NULL, ",")) == NULL) { 
				perror("strtok:");
				compteur_de_bug++;
				trace = C_POELE_SAC;
				return -1;
			}
			int nb_sac = atoi(tmpstr); 
			sprintf(szTrace, " receiver : msg poele SAC nb_sac:%d",nb_sac);
			syslog(LOG_DEBUG, szTrace);       
			put_into_pellet_database(table, entete, nb_sac);
		}
		//si c'est un message qu indique le niveau de granulés dans le reservoir
		else if(!strncmp(tmpstr, "NVG",3)){
			if((tmpstr = strtok (NULL, ",")) == NULL) { 
				perror("strtok:");
				compteur_de_bug++;
				trace = C_POELE_NVG;
				return -1;
			}
			int niveau_res = atoi(tmpstr); 
			sprintf(szTrace, " receiver : msg poele NVG niveau reservoir :%d",niveau_res);
			syslog(LOG_DEBUG, szTrace);       
			put_into_pelletres_database("pellets_rsv", entete, niveau_res);
		}
		//DEBUG
		printf("  -> enreg. dans base de donnees\n");
		fflush(stdout);
		// on retourne 0 car tout est ok
		return(0);
	}
}


//----------------------------------------------------------------------
//!\brief           Dépouille les infos du message $TND , position du robot tondeuse
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int trt_msg_tnd(char *message, char *table){
	printf("\n  Traitement msg %s", table);
	//***************************
	//* Log dans la base de données
	//***************************
	char *tmpstr;
	
	// entete = strtok (message, ",");
	// ana1 = atoi(strtok (NULL, ",")); 
	if((entete = strtok (message, ",")) == NULL) {
		perror("strtok:");
		compteur_de_bug++;
		trace = C_ENTETE;
		return -1;
	}
	if((tmpstr = strtok (NULL, ",")) == NULL) { 
		perror("strtok:");
		compteur_de_bug++;
		trace = C_TYPE_TND;
		return -1;
	}
	else {
		long dflat = atoi(tmpstr); 
		long dflon = atoi(strtok (NULL, ",")); 
		sprintf(szTrace, " receiver : Entete:%s, flat:%ld, flon:%ld", entete, dflat, dflon);
		syslog(LOG_DEBUG, szTrace);         
		if(put_into_tnd_database(table, entete, dflat, dflon) == 1){
			//DEBUG
			printf("  -> Erreur lors de l'ecriture en bdd");
			fflush(stdout);
			return(1);
		}
		else{
			//DEBUG
			printf("  -> enreg. dans base de donnees\n");
			fflush(stdout);
			// on retourne 0 car tout est ok
			return(0);
		}
	}
}


//----------------------------------------------------------------------
//!\brief           Lance le traitement adéquat en fonction du type de message
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int trt_message(void){
	//si on a recu un message valide
	if(bMessage_recu == TRUE){
		// en fonction du type de message
		switch(type_message){
		// on est dans le néant
		case MSG_NULL:
			printf("\n Message non reconnu");
			// on écrase le message
			message_pointer = 0;
			message[message_pointer] = 0;
			break;
		case MSG_TELEINFO:{
			char *table = "teleinfo";
			trt_msg_teleinfo(message, table);                    
			break;
			}
		case MSG_ANALOG1:{
			char *table = "analog1";
			trt_msg_analog(message, table);
			break;
			}
		case MSG_ANALOG2:{
			char *table = "analog2";
			trt_msg_analog(message, table);
			break;
			}
		case MSG_ANALOG3:{
			char *table = "analog3";
			trt_msg_analog(message, table);
			break;
			}
		case MSG_ANALOG4:{
			char *table = "analog4";
			trt_msg_analog(message, table);
			break;
			}
		case MSG_ANALOG5:{
			char *table = "analog5";
			trt_msg_analog(message, table);
			break;
			}
		case MSG_ANALOG6:{
			char *table = "analog6";
			trt_msg_analog(message, table);
			break;
			}
		case MSG_PYR:{
			char *table = "pyranometre";
			trt_msg_pyranometre(message, table);
			break;			
			}
		case MSG_POELE:{
			char *table = "pellets";
			trt_msg_poele(message, table);
			break;			
			}
		case MSG_VMC:{
			char *table = "VMC";
			trt_msg_analog(message, table);
			break;			
			}	
		case MSG_TND:{
			char *table = "tondeuse_pos";
			trt_msg_tnd(message, table);
			break;			
			}			
		}      
	  // flush pour afficher tous les messages et debug
	  fflush(stdout);
	  //on reset le flag
	  bMessage_recu = FALSE;
	  //on remet à 0 le time_out_portCOM
	  time_out_portCOM = 0;
	}
	return(0);
}


//----------------------------------------------------------------------
//!\brief          Compare l'entete du buffer reçu avec les différentes entete connus
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int compare_entete(void){

	//Déclaration des variables locales
	int jj;
	
	// Pour chaque caractère du buffer de récepttion
	for ( jj=0 ; jj<taille_msg ; jj++) {
		// si on détecte la fin d'un message
		if (buf[jj] == 0X0A){
			// on marque la fin de chaine puisque le message est complet
			message[message_pointer] = 0;
			printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~ %d", compteur_de_bug);
			
			if(trace != C_OK) {
				sprintf(szTrace, "receiver : ca a plante dans [%s]", aszTrace[trace]);
				syslog(LOG_DEBUG, szTrace);
				//on reset le flag pour éviter de saturer le log
				trace = C_OK;
			}
			//affiche la DATE
			//l ecture de la date/heure courante
			t = time(NULL);
			// conversion en heure locale
			local = localtime(&t);
			// création variables pour stocker la date et l'heure dans un format personnalisé
			char *datestamp = (char *)malloc(sizeof(char) * 10); //YYYY-MM-DD
			char *timestamp = (char *)malloc(sizeof(char) * 8); //hh:mm:dd
			// on prépare les chaines date et heure
			printf("\n%02d-%02d-%02d", local->tm_year+1900, local->tm_mon+1, local->tm_mday);
			printf(", %02d:%02d:%02d", local->tm_hour, local->tm_min, local->tm_sec);
			// affiche le message recu
			printf("\n %s", message);
			fflush(stdout);
			//On determine le type de message recu
			if(!strncmp(message, "$EDF",4))
				type_message = MSG_TELEINFO;
			else if(!strncmp(message, "$AN1",4))
				type_message = MSG_ANALOG1;
			else if(!strncmp(message, "$AN2",4))
				type_message = MSG_ANALOG2;
			else if(!strncmp(message, "$AN3",4))
				type_message = MSG_ANALOG3;		
			else if(!strncmp(message, "$AN4",4))
				type_message = MSG_ANALOG4;
			else if(!strncmp(message, "$AN5",4))
				type_message = MSG_ANALOG5;
			else if(!strncmp(message, "$AN6",4))
				type_message = MSG_ANALOG6;
			else if(!strncmp(message, "$PYR",4))
				type_message = MSG_PYR;
			else if(!strncmp(message, "$POL",4))
				type_message = MSG_POELE;
			else if(!strncmp(message, "$VMC",4))
				type_message = MSG_VMC;				
			else if(!strncmp(message, "$TND",4))
				type_message = MSG_TND;	
			else{
				type_message = MSG_NULL;
				printf("\n msg non reconnu : %s",message);
				}
				//on remet le flag de message valide recu a 0
				bMessage_recu = TRUE;
		}
		else {
			// on est à l'intérieur d'un message
			// on stocke le caractère en cours
			message[message_pointer] = buf[jj];
			// et on fait progresser le pointeur
			if(message_pointer < TAILLE_MESSAGE)
				message_pointer++;
			else{
				sprintf(szTrace, "receiver : index message_pointer > TAILLE_MESSAGE (255)");
				syslog(LOG_DEBUG, szTrace);
				message_pointer = 0;
			}
		}
	}
	return(0);
}


//----------------------------------------------------------------------
//!\brief           PROGRAMME PRINCIPAL
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int main(int argc, char *argv[]) {

	//***************************
	//* INITIALISATION
	//***************************
	printf("Installing signal handler...");
	(void) signal(SIGINT, sigfun);
	printf("done\n");  
	
	// initialisation du mécanisme syslog
	// LOG_CONS, on écrit sur la console en cas de défaillance de syslog
	openlog(log_name, LOG_CONS, LOG_USER);
	syslog(LOG_DEBUG, "receiver : Demarrage");

	// on configure et on ouvre le port COM
	if(ConfigOpenPortCOM(argc, argv) == 1) {
		// erreur nb de params ou ouverture du com port
		sprintf(szTrace, "receiver : Erreur à l'ouverture du portCOM");
		syslog(LOG_DEBUG, szTrace);
		closelog();
		exit(1);
	}

	//lecture de la date/heure courante
	t = time(NULL);
	// conversion en heure locale
	local = localtime(&t); 
	printf("Heure et date courante : %s\n", asctime(local));
	printf("Heure et date courante (UNIX): %d\n", t);
  
	//***************************************************************************
	//* BOUCLE PRINCIPALE , on boucle tant que CTRL+C n'est pas detecte
	//***************************************************************************
	while(keep_looping) {
		
		// returns after 1 chars have been input 
		// je ne suis pas d'accord, on lit jusqu'à 255 caractères !!!
		taille_msg = read(fd, buf, 255);
		// peut être
		// on pourrait faire ça :
		// on a lu aucun caractère
		if(taille_msg <= 0) {
			sleep(1);
			putchar('.');
			time_out_portCOM++;
			//si on passe 60 fois ici, il y a un pb sur le portCOM !
			if(time_out_portCOM > 120) {
				// on ferme le portCOM
				printf("On ferme le portCOM\n");
				close(fd);
				// on configure et on ouvre le port COM
				if(ConfigOpenPortCOM(argc, argv) == 1) {
					// erreur nb de params ou ouverture du com port
					sprintf(szTrace, "receiver : Erreur à l'ouverture du portCOM");
					syslog(LOG_DEBUG, szTrace);
					closelog();
					exit(1);
				}
				time_out_portCOM = 0;
				sprintf(szTrace, "receiver : reinitialisation du portCOM suite a expiration du time_out_portCOM > 60");
				syslog(LOG_DEBUG, szTrace);
				printf("\n(!!RESET PortCOM!!)\n");
			}
			fflush(stdout);
			// on boucle au while(keep_looping)
			continue;
		}
		
		int value = 0;

		// Si on détecte le début d'un message avec $
		if (buf[0] == '$') {
			// pointeur au début de la liste
			message_pointer = 0;
			// on met un fin de chaine au début de celle-ci pour avoir une chaine vide
			// ceci ne sert qu'à se faire plaisir puisqu'on va ensuite écraser cette valeur 
			// par le premier caractère reçu depuis la voie série :o)
			message[message_pointer] = 0;     
		}

		//on comparer l'entete du buffer reçu avec les entete connus
		compare_entete();

		//on effectue le traitement associé au type de message reçu
		trt_message();
		
	}	
	
	closelog();
	return (0);
}
