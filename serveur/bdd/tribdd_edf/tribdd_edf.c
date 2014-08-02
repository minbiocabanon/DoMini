//----------------------------------------------------------------------
//!\file          tribdd_edf_loop.c
//!\date	13/03/2012
//!\author    Minbiocabanonnon
//!\brief       Programme qui allège les tables de la BDD -> suppression de vieilles données avec filtrage.
//!\brief       Tri réalisé quotidiennement sur la table "teleinfo"
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
int tag_donnees(char *table, char *ordre);
int supp_donnees_tag(char *table);


#define _POSIX_SOURCE 1 // POSIX compliant source
#define FALSE 0
#define TRUE 1

#define NB_JOUR	8

int keep_looping = TRUE;

//déclaration de la table sur laquelle on va travailler
char table[]="teleinfo";


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
//!\brief           on tag les données particulières : min+max
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int tag_donnees(char *table, char *ordre){

	printf("\n\nOn lance les tags, ordre %s", ordre);
	// flush pour afficher tout les messages et debug
	fflush(stdout);
	
	// Preparation de la requete MySQL
	// on ne récupère que les infos de J-N
	sprintf(query, "SELECT id FROM `%s` WHERE date_format( date_time, '%%Y-%%d-%%m' ) = date_format( SUBDATE( NOW( ) , %d ) , '%%Y-%%d-%%m' ) ORDER BY `date_time` %s LIMIT 0 , 1", table, NB_JOUR, ordre);

	// envoi de la requete
	//printf("\nEnvoi de la requete : %s", query);
	if (mysql_query(conn, query)){
	// si la requete echoue on retourne 1
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
	}

	// la requête s'est bien passée, on rend le jeu de résultats disponible via le pointeur result
	//printf("\nRecuperation des donnees");
	result = mysql_store_result(conn);
	
	// si la requete retourne un résultat
	if (row = mysql_fetch_row(result)){

		//création de la variable qui va contenir l'id de la ligne à tagger
		// on converti le string en int
		int ID = atoi(row[0]);

		// on prépare la requete pour tagger la ligne trouvée
		sprintf(query, "UPDATE `domotique`.`%s` SET `tag` = '1' WHERE `%s`.`id` =%d;", table, table, ID);

		// envoi de la requete
		printf("\nEnvoi de la requete : %s", query);
		// flush pour afficher tout les messages et debug
		fflush(stdout);
		if (mysql_query(conn, query)){
		// si la requete echoue on retourne 1
			fprintf(stderr, "%s\n", mysql_error(conn));
			return(1);
		}		
	// si requete renvoi du vide
	}else{
		// probleme : aucune ligne retournée... MERDE! PAS NORMAL
	}
	// on libère la requête
	mysql_free_result(result);
	
	
	return(0);
}

//----------------------------------------------------------------------
//!\brief           on supprime les donnees que l'on a tagge precedemment
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int supp_donnees_tag(char *table){

	//message de debug
	printf("\nOn supprime toutes les lignes de la table %s qui ne sont pas taggees...", table);

	// Preparation de la requete MySQL
	// on ne supprime que les données de J-N
	sprintf(query, "DELETE FROM `domotique`.`%s` WHERE date_format( date_time, '%%Y-%%d-%%m' ) = date_format( SUBDATE( NOW( ) , %d ) , '%%Y-%%d-%%m') AND `%s`.`tag` =0", table, NB_JOUR, table);

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
	//initialisation BDD
	if(init_mysql() == 1) {
		// erreur de liaison avec la bdd
		// prévoir un msg de log vers un fichier de log
		exit(1);
	}

	//DEBUG
	printf("\n Table concernee : %s", table);
	
	//on tag la première valeur HP/HC du jour
	if(tag_donnees(table, "ASC") == 1) {
		// erreur de liaison avec la bdd
		// prévoir un msg de log vers un fichier de log
		exit(1);
	}
	
	//on tag la dernière valeur HP/HC du jour
	if(tag_donnees(table, "DESC") == 1) {
		// erreur de liaison avec la bdd
		// prévoir un msg de log vers un fichier de log
		exit(1);
	}	
		
	//on supprime les donnees que l'on a tagge precedemment
	if(supp_donnees_tag(table) == 1) {
		// erreur de liaison avec la bdd
		// prévoir un msg de log vers un fichier de log
		exit(1);
	}
	
	// fermeture de MySQL
	mysql_close(conn);
	
	// on affiche l'heure a nouveau, juste pour le debug et voir le temps pour executer les requetes
	t = time(NULL);
	local = localtime(&t);
	printf("\n\nHeure et date courante : %s\n", asctime(local));
			
	// flush pour afficher tout les messages et debug
	fflush(stdout);

	printf("\nEXIT \n\n");

	//exit(0);
}
