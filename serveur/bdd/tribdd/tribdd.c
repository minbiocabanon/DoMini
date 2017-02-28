//----------------------------------------------------------------------
//!\file          tribdd.c
//!\date	28/10/2011
//!\author    RzBo
//!\brief       Programme qui allège les tables de la BDD -> suppression de vieilles données avec filtrage.
//----------------------------------------------------------------------

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
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

// prototypes des fonctions utilisées
void sigfun(int sig);
int init_mysql(void);
int tag_donnees(char *table, int mois, char *ordre);
int tag_donnees_heure(char *table, int mois);
int supp_donnees_tag(char *table, int mois);
int getdayofmonth(void);
int getmonthnumber(void);
int getmonthdayscount(void);
int getnmonthdayscount(int);

#define _POSIX_SOURCE 1 // POSIX compliant source
#define FALSE 0
#define TRUE 1

int keep_looping = TRUE;
int seq, device_number; //generic data on package

// On déclare un pointeur long sur un non signé pour y stocker la taille des valeurs
unsigned long *lengths;

//déclaration de la table sur laquelle on va travailler
char table[50] = "VMC";
// déclaration le numéro du mois sur lequel on va travailler
int mois= 01;
int annee= 2017;

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
int tag_donnees(char *table, int mois, char *ordre){

	printf("\n\nOn lance les tags pour le mois %d, donnees %s", mois, ordre);
	
	int nbjourmois = getnmonthdayscount(mois);
	printf("\nnombre de jours du mois %d = %d", mois, nbjourmois);
	
	if(ordre == "MIN")
		ordre = "ASC";
	else if(ordre == "MAX")
		ordre = "DESC";
	else
		//erreur, on sort
		return(1);

	int i=1;
	// pour chaque jour du mois
	for(i=1 ; i<= nbjourmois ; i++){

		// on récupère la valeur maxi du jour
		// Preparation de la requete MySQL
		// on ne récupère que les infos du jour en cours
		sprintf(query, "SELECT id, date_time, ana1 FROM `%s` WHERE DAY( date_time ) = %d AND MONTH( date_time ) = %d AND YEAR( date_time ) = %d ORDER BY ana1 %s LIMIT 0 , 1", table, i, mois, annee, ordre);

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
			//printf("\nEnvoi de la requete : %s", query);
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
	}
	
	return(0);
}

//----------------------------------------------------------------------
//!\brief           on supprime les donnees que l'on a tagge precedemment
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int tag_donnees_heure(char *table, int mois){

	printf("\nOn lance les tags horaires pour le mois %d", mois);
	
	int nbjourmois = getnmonthdayscount(mois);
	printf("\nnombre de jours du mois %d = %d\n", mois, nbjourmois);
	
	int i=1;
	// pour chaque jour du mois
	for(i=1 ; i<= nbjourmois ; i++){
		int h=0;
		//pour chauque heure du jour
		for(h=0; h<=23; h++){
			// on récupère la valeur de l'heure "pleine"
			// Preparation de la requete MySQL
			// on ne récupère que les infos du jour en cours
			sprintf(query, "SELECT id, date_time, ana1 FROM `%s` WHERE HOUR(date_time) = %d AND DAY( date_time ) = %d AND MONTH( date_time ) = %d AND YEAR( date_time ) = %d ORDER BY date_time ASC LIMIT 0 , 1", table, h, i, mois, annee);

			// envoi de la requete
			//printf("\nRequete pour le jour %d , heure %d", i, h);
			printf(".");fflush(stdout);
			
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
				//printf("\nEnvoi de la requete : %s", query);
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
		}
	}
	return(0);
}

//----------------------------------------------------------------------
//!\brief           on supprime les donnees que l'on a tagge precedemment
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int supp_donnees_tag(char *table, int mois){

	//message de debug
	printf("\nOn supprime toutes les lignes de la table %s du mois %d qui ne sont pas taggees...", table, mois);

	// on récupère la valeur maxi du jour
	// Preparation de la requete MySQL
	// on ne récupère que les infos du jour en cours
	sprintf(query, "DELETE FROM `domotique`.`%s` WHERE month( date_time ) =%d AND `%s`.`tag` =0", table, mois, table);

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
	
		
}

//----------------------------------------------------------------------
//! \brief retourne le jour du mois
//----------------------------------------------------------------------
int getdayofmonth(void) {
  struct timeval stTv;
  struct tm *pstTm;

  gettimeofday(&stTv,NULL);
  pstTm = localtime(&stTv.tv_sec);
  return(pstTm->tm_mday);
}


//----------------------------------------------------------------------
//! \brief retourne le numero du mois
//----------------------------------------------------------------------
int getmonthnumber(void) {
  struct timeval stTv;
  struct tm *pstTm;

  gettimeofday(&stTv,NULL);
  pstTm = localtime(&stTv.tv_sec);
  return((pstTm->tm_mon) + 1);
}

//----------------------------------------------------------------------
//! \brief retourne le nombre de jours du mois passe en param
//----------------------------------------------------------------------
int getnmonthdayscount(int _nMonth) {
  struct timeval stTv;
  struct tm *pstTm;
  int nMonth = 0;

  gettimeofday(&stTv,NULL);
  pstTm = localtime(&stTv.tv_sec);
  pstTm->tm_mon = _nMonth - 1;
  nMonth = pstTm->tm_mon;
  if(nMonth == 11) {
    nMonth = 0;
    pstTm->tm_year += 1;
  }
  else {
    nMonth += 1;
  }
  pstTm->tm_mon = nMonth;
  pstTm->tm_mday = 0;

  mktime(pstTm);

  return(pstTm->tm_mday);
}

//----------------------------------------------------------------------
//! \brief retourne le nombre de jours du mois
//----------------------------------------------------------------------
int getmonthdayscount(void) {
  struct timeval stTv;
  struct tm *pstTm;
  int nMonth = 0;

  gettimeofday(&stTv,NULL);
  pstTm = localtime(&stTv.tv_sec);
  nMonth = pstTm->tm_mon;
  if(nMonth == 11) {
    nMonth = 0;
    pstTm->tm_year += 1;
  }
  else {
    nMonth += 1;
  }
  pstTm->tm_mon = nMonth;
  pstTm->tm_mday = 0;

  mktime(pstTm);

  return(pstTm->tm_mday);
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


	//initialisation BDD
	if(init_mysql() == 1) {
		// erreur de liaison avec la bdd
		// prévoir un msg de log vers un fichier de log
		exit(1);
	}
	
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
	// on cherche le numéro du mois précédent
	//mois = getmonthnumber() - 1;

	// printf("jour du mois : %d\n",   getdayofmonth());
	// printf("numero du mois : %d\n", getmonthnumber());
	//printf("nombre de jours du mois : %d\n", getmonthdayscount());
	
	  
	//***************************************************************************
	//* PROGRAMME PRINCIPAL 
	//***************************************************************************
	
	//on tag les données MIN par jour
	if(tag_donnees(table, mois,"MIN") == 1) {
		// erreur de liaison avec la bdd
		// prévoir un msg de log vers un fichier de log
		exit(1);
	}
	
	//on tag les données MAX par jour
	if(tag_donnees(table, mois,"MAX") == 1) {
		// erreur de liaison avec la bdd
		// prévoir un msg de log vers un fichier de log
		exit(1);
	}
	
	//on tag les données horaire par jour
	if(tag_donnees_heure(table, mois) == 1) {
		// erreur de liaison avec la bdd
		// prévoir un msg de log vers un fichier de log
		exit(1);
	}
	
	//on supprime les donnees que l'on a tagge precedemment
	if(supp_donnees_tag(table, mois) == 1) {
		// erreur de liaison avec la bdd
		// prévoir un msg de log vers un fichier de log
		exit(1);
	}
		
	// flush pour afficher tout les messages et debug
	fflush(stdout);
	// fermeture de MySQL
	mysql_close(conn);
	printf("\nEXIT \n\n");

	//exit(0);
}
