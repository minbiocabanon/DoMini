//----------------------------------------------------------------------
//!\file         del_old_data.c
//!\date	23/03/2012
//!\author    minbiocabanon
//!\brief       Programme qui supprime les données d'une date selon un nombre de jours
//!\brief       Tri réalisé quotidiennement
//----------------------------------------------------------------------

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
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

// prototypes des fonctions utilisées
void sigfun(int sig);
int init_mysql(void);
int supp_donnees(char *table);


#define _POSIX_SOURCE 1 // POSIX compliant source
#define FALSE 0
#define TRUE 1

#define NB_JOUR	15

int keep_looping = TRUE;

//déclaration de la table sur laquelle on va travailler
char *table[]={"chauffage_log","voletroulant_log"};
#define NB_ELEMENT_TABLE	2

// Déclaration du pointeur de structure de type MYSQL
MYSQL *conn;
MYSQL_RES *result = NULL;
MYSQL_ROW row = NULL;

//----------------------------------------------------------------------
//!\brief           trap du control C
//!\param[in]     sig paramètre qui ne sert à rien
//----------------------------------------------------------------------
void sigfun(int sig) {
    printf("You have pressed Ctrl-C , aborting!\n");
    keep_looping = FALSE;
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
  //printf("Envoi de la requete USE DOMOTIQUE\n");
  if (mysql_query(conn, "USE domotique")) {
    fprintf(stderr, "%s\n", mysql_error(conn));
    return(1);
  }
  // s'il n'y a pas d'erreurs, tout est ok, on le dit
  return(0);
}

//----------------------------------------------------------------------
//!\brief           on supprime les donnees plus vieille que le jour J
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int supp_donnees(char *table){

	//message de debug
	printf("\nOn supprime toutes les lignes de la table %s plus vieille de %d jour(s))", table, NB_JOUR);

	// Preparation de la requete MySQL
	// on ne supprime que les données de J-7
	sprintf(query, "DELETE FROM `domotique`.`%s` WHERE `date_time` < DATE_SUB(CURDATE(), interval %d DAY)", table, NB_JOUR);

	// envoi de la requete
	//printf("\nEnvoi de la requete : %s", query);
	if (mysql_query(conn, query)){
	// si la requete echoue on retourne 1
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
	}

	// on lance une optimisation de la table afin de défragementer les données
	printf("\nOptimisation de la table en cours ...");
	sprintf(query, "OPTIMIZE TABLE `%s`", table);
	// envoi de la requete
	//printf("\nEnvoi de la requete : %s", query);
	if (mysql_query(conn, query)){
	// si la requete echoue on retourne 1
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
	}
		
	return(0);
		
}

//***********************************************
//----------------------------------------------------------------------
//!\brief           le main !
//----------------------------------------------------------------------
//***********************************************
int main() {  

	//***************************
	//* INITIALISATION
	//***************************
	printf("Installing signal handler...");
	(void) signal(SIGINT, sigfun);
	printf("done\n");


	//lecture de la date/heure courante
	// on récupère l'heure courante au format UNIX -> nb de secondes depuis le 01/01/1970 , je fais comme ça car c'est plus facile que de comparer des dates/heure
	// seconds = nb de secondes depuis le 1er janvier 1970
	// conversion en heure locale
	struct tm *local;
	time_t t;
	t = time(NULL);
	// conversion en heure locale
	local = localtime(&t);
	printf("Heure et date courante : %s\n", asctime(local));
	
	  
	//***************************************************************************
	//* PROGRAMME PRINCIPAL 
	//***************************************************************************
	int i;
	
	for(i=0; i< NB_ELEMENT_TABLE; i++ ){
	
		//initialisation BDD
		if(init_mysql() == 1) {
			// erreur de liaison avec la bdd
			// prévoir un msg de log vers un fichier de log
			exit(1);
		}
	
		//DEBUG
		printf("\n Table concernee : %s", table[i]);
		
		//on supprime les donnees
		if(supp_donnees(table[i]) == 1) {
			// erreur de liaison avec la bdd
			// prévoir un msg de log vers un fichier de log
			exit(1);
		}
		
		// fermeture de MySQL
		mysql_close(conn);
	
	}
		
	// flush pour afficher tout les messages et debug
	fflush(stdout);

	printf("\nEXIT \n\n");

	//exit(0);
}
