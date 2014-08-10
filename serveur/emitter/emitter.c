//----------------------------------------------------------------------
//!\file          emitter.c
//!\date
//!\author    RdZBo
//!\brief       Simple C program that connects to MySQL Database server
//----------------------------------------------------------------------

// 
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

#include <string.h>
#include <stddef.h>
#include "passwd.h"

#define BAUDRATE B57600
#define _POSIX_SOURCE 1 // POSIX compliant source
#define FALSE 0
#define TRUE 1

#define row_id 0
#define row_date_time 1
#define row_date_unix 2
#define row_message 3

#define TIMEOUTMSG 120 //en secondes

// prototypes des fonctions utilisées
void sigfun(int sig);
int ConfigOpenPortCOM(int argc, char *argv[]);
int init_mysql();
int recup_pile_msg();
void read_date_msg();
int send_msg_to_radio();
int delete_msg();


// Declaration des variables 
int keep_looping = TRUE;

int fd;
struct termios oldtio,newtio;

int iTaille_msg = 0;
char message[255] = "still empty";
char date_msg[]="1970-01-01 00:00:00";

// On déclare un pointeur long sur un non signé pour y stocker la taille des valeurs
unsigned long *lengths;

// pour la gestion de l'heure/date
time_t t;
struct tm *local;
unsigned int time_unix_msg;

// Déclaration du pointeur de structure de type MYSQL
MYSQL *conn;
MYSQL_RES *result = NULL;
MYSQL_ROW row = NULL;
// nom de la table mysql concernée
char *table = "tx_msg_radio";

//----------------------------------------------------------------------
//!\brief           trap du control C
//!\param[in]     sig paramètre qui ne sert à rien
//----------------------------------------------------------------------
void sigfun(int sig) {
    printf("You have pressed Ctrl-C , aborting!\n");
    keep_looping = FALSE;
	
	// libération du jeu de résultat
	mysql_free_result(result);

	// fermeture de MySQL
	mysql_close(conn);
	
    exit(0);
}


//----------------------------------------------------------------------
//!\brief           ouverture et configuration du port com
//!\param[in]     argc nombre de paramètres reçus depuis le main()
//!\param[in]     argv liste des paramètres du main
//!\return        -1 si le nb de params est incorrect
//----------------------------------------------------------------------
int ConfigOpenPortCOM(int argc, char *argv[]){

  if(argc != 2) {
    printf("Incorrect number of args: usage arduinomonitor <USB port> e.g. /dev/ttyUSB0\n");
    return(-1);
  }
  printf("connecting to port %s\n", argv[1]);

	// ouverture du port série  en write only
	// l'option O_RDWR fait une ouverture en read/write
	fd = open(argv[1], O_WRONLY | O_NONBLOCK | O_NOCTTY );
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
	newtio.c_cflag |= PARENB;
	newtio.c_cflag |= IGNPAR; // Pas de Parité
	newtio.c_cflag &= CSTOPB; // 1 stop bit
	
	// bzero(&newtio, sizeof(newtio));
	// newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
	// newtio.c_iflag = IGNPAR;
	// newtio.c_oflag = 0;

	// set input mode (non-canonical, no echo,...) 
	newtio.c_lflag = 0;

	// newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
	// newtio.c_cc[VMIN]     = 1;   /* blocking read until 1 chars received */

	tcflush(fd, TCIFLUSH);
	// on rend effectif le paramétrage du port série
  
	if( tcsetattr(fd,TCSANOW,&newtio) < 0 ){
		perror ("tcsetattr");
		// on sort
		return(-1);
	}
  
	// tout s'est bien passé
	return(0);
}


//----------------------------------------------------------------------
//!\brief           ouverture d'une connexion à la bdd
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int init_mysql(){

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
//!\brief           récupération des messages dans la bdd
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int recup_pile_msg(){

  // Preparation de la requete MySQL
  // on ne récupère que les messages de la table dont la date n'est pas antérieure à 30 secondes (penser à supprimer les autres!!!!)
  //sprintf(query, "SELECT id,UNIX_TIMESTAMP(date_time),message FROM %s WHERE date_time <= NOW( ) AND date_time >= SUBTIME( NOW( ) ,  '48:30:00' ) ORDER BY date_time DESC", table);
  // on récupère toute la table (on fera le tri par soft aprés...)
  sprintf(query, "SELECT id, date_time, UNIX_TIMESTAMP(date_time) AS date_unix, message FROM %s ORDER BY date_time ASC", table);

  // envoi de la requete
  printf("Envoi de la requete : %s\n", query);
  if (mysql_query(conn, query)) {
    // si la requete echoue on retourne 1
    fprintf(stderr, "%s\n", mysql_error(conn));
    return(1);
  }

  // la requête s'est bien passée, on rend le jeu de résultats disponible via le pointeur result
  printf("Recuperation des donnees\n");
  result = mysql_use_result(conn);

  // on récupère le nombre de champs
  // unsigned int num_champs = mysql_num_fields(result);
  // printf("Recuperation du nombre de champs de la table: %d\n",num_champs);
  fflush(stdout);
  return(0);
}


//----------------------------------------------------------------------
//!\brief           lit la date du message en cours et convertit en timestamp
//----------------------------------------------------------------------
void read_date_msg() {
  
  // on récupère la date du message lu dans la pile
  sprintf(date_msg,"%.*s", (int) lengths[row_date_unix], row[row_date_unix]);
  
  // on affiche cette date (au format UNIX) pour la comparaison future avec la date courante
  printf ("\n date_msg : %s \n", date_msg);
  
  //conversion de la date (string) en entier
  time_unix_msg = atoi(date_msg);
  
  fflush(stdout);
}


//----------------------------------------------------------------------
//!\brief           envoi du message en cours par radio
//!\return        taille du message envoyé, 0 si erreur
//----------------------------------------------------------------------
int send_msg_to_radio() {

  // on récupère le message à emettre dans la pile
  sprintf(message,"%.*s\n", (int) lengths[row_message], row[row_message]);        
  // on récupère la taille du message à émettre
  iTaille_msg = strlen (message);

  // si l'ouverture est effective on envoie le message
	int i=0;
	for (i=0; i< iTaille_msg; i++)
	{     
	 char cCar_emis = message[i];
	 printf ("%c", cCar_emis); 
	 if (write (fd, &cCar_emis, 1) <0)
		  perror ("write");
	}

  // if(write(fd, &message, iTaille_msg) < 0) {
    // //sinon on affiche un msg d'erreur
    // perror ("impossible d'envoyer sur le port COM");
    // return(0);
  // }
    
	// DEBUG
	printf ("\nMessage envoye : %s \n", message);
	fflush(stdout);
  
	//petite pause avant d'envoyer le prochain message...
	sleep(5);
  
  return(iTaille_msg);
}


//----------------------------------------------------------------------
//!\brief           effacement du message traité
//!\return        0 si ok, 1 si erreur
//----------------------------------------------------------------------
int delete_msg() {

  // on prépare la requete MySQL
  sprintf(query, "TRUNCATE TABLE `tx_msg_radio`"); // Efface toute la table
  //Envoi de la requete
   if (mysql_query(conn, query)) {
    // si la requete echoue on retourne le message d'erreur
    fprintf(stderr, "%s\n", mysql_error(conn));
    return(1);
  }
  printf ("\nMessage(s) supprime(s) de la base \n");
  fflush(stdout);
  return(0);
}


//----------------------------------------------------------------------
//!\brief           le main !
//----------------------------------------------------------------------
int main(int argc, char *argv[]) {  
  
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
  printf("Heure et date courante (UNIX): %d\n", t);
  
  
  // récupération de la pile des messages 
  if(recup_pile_msg() == 1) {
    // erreur sur la recuperation des messages
    // prévoir un msg de log vers un fichier de log
    exit(1);
  }
  
  // pour chaque message
  while ((row = mysql_fetch_row(result))) {
  
    // on stocke la taille  dans le pointeur
    lengths = mysql_fetch_lengths(result);
    
    // on lit la date de création du message lu dans la pile
    read_date_msg();
      
    //si la date est récente ( <1 minute)
    if(t - time_unix_msg < TIMEOUTMSG) {
	
	  // on configure et on ouvre le port COM
	  if(ConfigOpenPortCOM(argc, argv) == 1) {
		// erreur nb de params ou ouverture du com port
		// prévoir un msg de log vers un fichier de log
		exit(1);
	  }
	
      // on envoie ce message vers la radio
      if(send_msg_to_radio() == 0) {
        // erreur sur la recuperation des messages
		printf("\nErreur sur envoi message, taille = %d", iTaille_msg);
        // prévoir un msg de log vers un fichier de log
      }
	  
		//fermeture du port COM
		close(fd);
	  
    }
    else {
      // on affiche un message d'avertissement
      printf("Message expire : %d sec.\n",t - time_unix_msg);
    }	
  }
  
	// on supprime tous les messages (dans tous les cas)
	if(delete_msg() == 1) {
		// erreur sur la recuperation des messages
		// prévoir un msg de log vers un fichier de log
	}    
  
  // libération du jeu de résultat
  mysql_free_result(result);

  // fermeture de MySQL
  mysql_close(conn);

  //exit(0);
}
