//----------------------------------------------------------------------
//!\file    regul_temp.c
//!\date	25/09/2011
//!\author  Minbiocabanon
//!\brief   Programme qui réalise la fonction thermostat (centralisé) du poêle
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
#include <dirent.h> //gestion des repertoires
#include <signal.h>
#include <time.h>
#include <string.h>
#include <stddef.h>
#include <syslog.h>
#include "regul_temp.h"
#include "passwd.h"

#define _POSIX_SOURCE 1 // POSIX compliant source
#define FALSE 0
#define TRUE 1

#define ROW_MODE 1
#define POELE_STOP -1
#define DELTA_TEMP_MAX 1.25				// en °C : Ecart maxi entre la consigne et la température mesuree au dela duquel on arrête le poele

// paramètre mode ANTICIPE
#define COEF_THERM_MAISON 0.0396		// en °C / min / %
#define P_POELE_ANTICIPE 75.0			// puissance de chauffe du poele en mode ANTICIPE en % , valeur par défaut si un problème survient du calcul de la consigne dynamique (fonction des températures relevées)
#define P_POELE_OFFSET	20.0			// Offset pour la puissance de chauffe en mode anticipé.
#define INERTIE_MAISON 45.0 			// Temps de retard en minutes entre le démarrage du poêle et le début de la montée en température de la maison

// Paramètre du régulateur PID
//#define Kp 1
#define Ki 0.1
#define Kd 5

// #define Kp 8.3
// #define Ti 108
// #define Td 15

#define Kp 40.8755
#define Ti 93
#define Td 14.5161

#define dt 5	// en minutes

int keep_looping = TRUE;
char log_name[] = "regul_temp";
char datestring[] = "YYYY-MM-DD HH:mm:ss";

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

  syslog(LOG_DEBUG, "--init_mysql, With MySQL support");

  //Initialisation liaison avec mysql
  conn = mysql_init(NULL);
    
  // Connexion à la BDD
  syslog(LOG_DEBUG, "Connexion a la BDD\n");
  
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
//!\brief           Envoi d'un message au poele avec consigne
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int envoie_msg_poele(int consigne_p){
	syslog(LOG_DEBUG, "--envoie_msg_poele");
	//on crée la variale qui contient l'état du poele ON ou OFF
	char poele_ON_OFF[] = "0";	// OFF par défaut

	//on test si le poele est ON ou OFF , pour OFF consigne_p= -1
	if(consigne_p == POELE_STOP){
		sprintf(poele_ON_OFF,"0");
		consigne_p = 0;
	}else{
		sprintf(poele_ON_OFF,"1");
	}

	// on stocke la valeur dans la structure pour le log
	stDonnees.puissance_chauffe = consigne_p;
	
	// Preparation de la requete MySQL
	// on utilise un '!' en entete de message pour demander un ack (géré par le Jeelink)
	sprintf(query, "INSERT INTO `domotique`.`tx_msg_radio` (`id` ,`date_time` ,`message` )VALUES (NULL , NOW(), '!POL,%s,%d');",poele_ON_OFF, consigne_p);

	// envoi de la requete
	if (mysql_query(conn, query)) {
		// si la requete echoue on retourne 1
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
	}
	
	syslog(LOG_DEBUG, "  Message envoye au poele : !POL,%s,%d",poele_ON_OFF, consigne_p);
	
	// printf("\nMessage ecrit dans la pile");
	// s'il n'y a pas d'erreurs, tout est ok, on le dit
	return(0);
}

//----------------------------------------------------------------------
//!\brief           Récupération de l'etat en cours de la machine d'état
//!\return        -1 si erreur, "etat" sinon
//----------------------------------------------------------------------
int get_etat(void){
	syslog(LOG_DEBUG, "--get_etat");
	// Preparation de la requete MySQL
	// on ne récupère que les infos du jour en cours
	sprintf(query, "SELECT `id`, `etat`, `etat_prec`, `mode`, `mode_prec`, `consigne_utilisateur`, UNIX_TIMESTAMP(`heure_debut`), `consigne_anticipe` FROM `chauffage_statut`");

	// envoi de la requete
	// printf("\nEnvoi de la requete : %s", query);
	if (mysql_query(conn, query)) {
		// si la requete echoue on retourne 1
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(ETAT_ERREUR);
	}

	// la requête s'est bien passée, on rend le jeu de résultats disponible via le pointeur result
	//printf("\nRecuperation des donnees");
	result = mysql_use_result(conn);

	//si aucune valeur retournée
	// si la requête ne retourne rien (arrive à la première requete aprés la création de la table... ou suite à bug)
	// on récupère le résultat (une seule ligne en principe... si la table est bien faite)
	if (!(row = mysql_fetch_row(result))){
		// probleme : ETAT non défini... on arrête TOUT !
		// on met l'ETAT OFF par défaut car c'est le plus stable
		stDonnees.etat = ETAT_OFF;
		syslog(LOG_DEBUG, "  ETAT forcé à OFF");
		
		// on envoie le message STOP au poele... au cas ou !
		envoie_msg_poele(POELE_STOP);
		// on libère la requête
		mysql_free_result(result);
		return(stDonnees.etat);
	// si la requete retourne un résultat
	}else{
			
		//création de la variable  qui va contenir l'etat contenu dans la base
		// on converti le string en float
		stDonnees.etat = atoi(row[TB_chauffage_status_ETAT]);
		//création de la variable  qui va contenir l'etat précédent contenu dans la base
		// on converti le string en float
		stDonnees.etat_prec = atoi(row[TB_chauffage_status_ETAT_PREC]);
		//création de la variable  qui va contenir la consigne utilisateur contenu dans la base
		// on converti le string en integer
		stDonnees.consigne_utilisateur = atoi(row[TB_chauffage_status_CONSIGNE_UTILISATEUR]);
		// on récupère l'heure de début de consigne de chauffe
		stDonnees.heure_debut_consigne = atoi(row[TB_chauffage_status_heure_debut_consigne]);
		//création de la variable  qui va contenir la consigne anticipée
		// on converti le string en integer
		stDonnees.consigne_anticipe = atoi(row[TB_chauffage_status_CONSIGNE_ANTICIPE]);

		// on libère la requête
		mysql_free_result(result);

		// on affiche l'état en cours au format INT
		syslog(LOG_DEBUG, "  etat precedent : %d", stDonnees.etat_prec);
		// on affiche l'état en cours au format INT
		syslog(LOG_DEBUG, "  etat en cours: %d", stDonnees.etat);
		// on affiche la consigne utilisateur récupérée
		syslog(LOG_DEBUG, "  Consigne utilisateur : %d", stDonnees.consigne_utilisateur);
		// on affiche l'heure de debut de consigne
		unixtime_to_date(stDonnees.heure_debut_consigne);
		syslog(LOG_DEBUG, "  Heure de debut de consigne : %d - %s", stDonnees.heure_debut_consigne, datestring);
		// on affiche la consigne utilisateur récupérée
		syslog(LOG_DEBUG, "  Consigne anticipee : %d", stDonnees.consigne_anticipe);

		// s'il n'y a pas d'erreurs, tout est ok, on le dit
		return(stDonnees.etat);
	}
	
	// pour forcer l'affichage du debug
	fflush(stdout);

}

//----------------------------------------------------------------------
//!\brief           sauvegarde l'heure de début de consigne du prochain créneau de chauffe dans la table domotique.chauffage_statut
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int sauve_heure_debut_consigne(unsigned int heure){
	syslog(LOG_DEBUG, "--sauve_heure_debut_consigne");
	// Preparation de la requete MySQL
	sprintf(query, "UPDATE `domotique`.`chauffage_statut` SET `heure_debut` = FROM_UNIXTIME(%d) WHERE `chauffage_statut`.`id` =1;", heure);	// envoi de la requete

	// envoi de la requete
	// printf("\nEnvoi de la requete : %s", query);
	if (mysql_query(conn, query)) {
		// si la requete echoue on retourne 1
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
	}
	syslog(LOG_DEBUG, "Heure debut de chauffe ecrit dans la base.", heure);
	// s'il n'y a pas d'erreurs, tout est ok, on le dit
	return(0);
}

//----------------------------------------------------------------------
//!\brief		converti un int en date
//!\return		string avec la date au format 2013-11-06 17:30:00
//----------------------------------------------------------------------
int unixtime_to_date(unsigned int unixstamp){
	// Preparation de la requete MySQL
	sprintf(query, "SELECT FROM_UNIXTIME( %d )", unixstamp);	// envoi de la requete

	// envoi de la requete
	// printf("\nEnvoi de la requete : %s", query);
	if (mysql_query(conn, query)) {
		// si la requete echoue on retourne 1
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
	}
	
	// la requête s'est bien passée, on rend le jeu de résultats disponible via le pointeur result
	//printf("\nRecuperation des donnees");
	result = mysql_use_result(conn);
	
	// si aucune valeur retournée
	// si la requête ne retourne rien (arrive à la première requete aprés la création de la table... ou suite à bug)
	// on récupère le résultat (une seule ligne en principe... si la table est bien faite)
	if (!(row = mysql_fetch_row(result))){
		// probleme : la requete n'a rien retourné	
		mysql_free_result(result);
		syslog(LOG_DEBUG, "  Erreur de requete MySQL");
		return(0);
		
	}else{// si la requete retourne un résultat
		syslog(LOG_DEBUG, "  Requete MySQL OK");
		// on retourne la date au format string
		//printf("\n #### date convertie %s \n\n", row[0]);
		sprintf(datestring, row[0]);
		// on libère la requête
		mysql_free_result(result);
		return(1);
	}
}

//----------------------------------------------------------------------
//!\brief           Gestion de l'état OFF
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int trt_ETAT_OFF(void){
	syslog(LOG_DEBUG, "--trt_ETAT_OFF, On est en mode OFF -> On éteint le poele");

	//si l'état précédent était OFF
	if(stDonnees.etat_prec == ETAT_OFF){		
		//on envoie le message STOP au poele
		envoie_msg_poele(POELE_STOP);		
		// on sort
		return(0);
	}else{ // sinon
		//on envoie le message STOP au poele
		envoie_msg_poele(POELE_STOP);
		//on change les états
		change_etat(ETAT_OFF);
		//on change le mode
		change_mode(MODE_ATTENTE);
	}
	// s'il n'y a pas d'erreurs, tout est ok, on le dit
	return(0);
}

//----------------------------------------------------------------------
//!\brief           Gestion de l'état ON
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int trt_ETAT_ON(void){
	syslog(LOG_DEBUG, "--trt_ETAT_ON");
	//si l'état précédent était ON
	if(stDonnees.etat_prec == ETAT_ON){
		//on envoie le message ON au poele avec la consigne utilisateur en paramètre
		envoie_msg_poele(stDonnees.consigne_utilisateur);
		// on sort 
		return(0);
	}else{ // sinon
		//on envoie le message ON au poele avec la consigne utilisateur en paramètre
		envoie_msg_poele(stDonnees.consigne_utilisateur);
		//on change les états
		change_etat(ETAT_ON);	
	}
	// s'il n'y a pas d'erreurs, tout est ok, on le dit
	return(0);
}

//----------------------------------------------------------------------
//!\brief           Machine d'état pour le mode Automatique
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int trt_ETAT_AUTO(void){
	syslog(LOG_DEBUG, "--trt_ETAT_AUTO");
	//si l'état précédent était différent de AUTO
	if(stDonnees.etat_prec != ETAT_AUTO)
		//on passe à l'état AUTO
		change_etat(ETAT_AUTO);

	//on récupère le mode en cours
	get_mode();
	
	//on récupère l'heure de démarrage
	get_h_dem();
		
	// en fonction de la machine d'état
	switch(stDonnees.mode){
	// on est  à l'arrêt, on va chercher si le poele va devoir chauffer...
	case MODE_ERREUR:
		syslog(LOG_DEBUG, "MODE ERREUR ... Que faire?");
		break;
	case MODE_ATTENTE:
		trt_mode_attente();
		break;
	// on a un créneau à chauffer qui arrive, on estime combien de temps avant il faut démarrer
	case MODE_ANTICIPE:
		trt_mode_anticipe();                    
		break;
	// ça y est, ça chauffe... on gère la puissance pour le confort des habitants
	case MODE_CHAUFFE:
		trt_mode_chauffe();
		break;
	}
	// s'il n'y a pas d'erreurs, tout est ok, on le dit
	return(0);
}

//----------------------------------------------------------------------
//!\brief           Récupération du mode en cours de la machine d'état
//!\return        -1 si erreur, "mode" sinon
//----------------------------------------------------------------------
int get_mode(void){
	syslog(LOG_DEBUG, "--get_mode");
	// Preparation de la requete MySQL
	// on ne récupère que les infos du jour en cours
	sprintf(query, "SELECT * FROM `chauffage_statut`");

	// envoi de la requete
	// printf("\nEnvoi de la requete : %s", query);
	if (mysql_query(conn, query)) {
		// si la requete echoue on retourne 1
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(-1);
	}

	// la requête s'est bien passée, on rend le jeu de résultats disponible via le pointeur result
	//printf("\nRecuperation des donnees");
	result = mysql_use_result(conn);

	//si aucune valeur retournée
	if (!(row = mysql_fetch_row(result))){
		// probleme : MODE non défini... on arrête TOUT !
		// on met le mode STOP par défaut car c'est le plus stable
		stDonnees.mode = 0;
		syslog(LOG_DEBUG, "  Mode forcé à 0, on envoie STOP au poele");
		// on envoie le message STOP au poele... au cas ou !
		envoie_msg_poele(POELE_STOP);
		// on libère la requête
		mysql_free_result(result);
		return(stDonnees.mode);
	// si la requete retourne un résultat
	}else{
		//création de la variable string qui va contenir le mode contenu dans la base
		// on converti le string en float
		stDonnees.mode = atoi(row[TB_chauffage_status_MODE]);
		//création de la variable string qui va contenir le mode précédent contenu dans la base
		// on converti le string en float
		stDonnees.mode_prec = atoi(row[TB_chauffage_status_MODE_PREC]);
		// on affiche l'état en cours au format INT
		syslog(LOG_DEBUG, "  mode precedent : %d", stDonnees.mode_prec);
		// on affiche l'état en cours au format INT
		syslog(LOG_DEBUG, "  mode en cours: %d", stDonnees.mode);
		// on libère la requête
		mysql_free_result(result);
		// s'il n'y a pas d'erreurs, tout est ok, on le dit
		return(stDonnees.mode);
	}
		
	// pour forcer l'affichage du debug
	fflush(stdout);

}

//----------------------------------------------------------------------
//!\brief           Récupération de l'heure de démarrage pour remplir le log et afficher sur l'ihm web
//!\return        -1 si erreur, "heure de démarrage en unixtime" sinon
//----------------------------------------------------------------------
int get_h_dem(void){

	//Si on est en mode > ANTICIPE, on récupère l'heure de démarrage (utile pour le log et l'affichage sur l'ihm web)
	if(stDonnees.mode >= MODE_ANTICIPE){
		syslog(LOG_DEBUG, " ---recup de l'heure de démarrage");
		// Preparation de la requete MySQL
		// on ne récupère que les infos du jour en cours
		sprintf(query, "SELECT UNIX_TIMESTAMP( `heure_demarrage` ) FROM `chauffage_log` ORDER BY `date_time` DESC Limit 1,1");

		// envoi de la requete
		// printf("\nEnvoi de la requete : %s", query);
		if (mysql_query(conn, query)) {
			// si la requete echoue on retourne 1
			fprintf(stderr, "%s\n", mysql_error(conn));
			return(-1);
		}

		// la requête s'est bien passée, on rend le jeu de résultats disponible via le pointeur result
		//printf("\nRecuperation des donnees");
		result = mysql_use_result(conn);

		//si aucune valeur retournée
		if (!(row = mysql_fetch_row(result))){
			// probleme : pas de log !!!!!!!??????? on arrête TOUT !
			// on ne fait rien, tant pis pour le log ou l'affichage.
			// on libère la requête
			mysql_free_result(result);
			return(-1);
		// si la requete retourne un résultat
		}else{
			//création de la variable string qui va contenir l'heure de démarrage contenu dans la base
			// on converti le string en float
			stDonnees.heure_demarrage = atoi(row[0]);

			// on affiche l'état en cours au format INT
			syslog(LOG_DEBUG, "  H. de démarrage lue dans le log (UNIXTIME): %d", stDonnees.heure_demarrage);
			// on libère la requête
			mysql_free_result(result);
		}	
	}
	else{
		stDonnees.heure_demarrage = 0;
	}
	
	return(stDonnees.heure_demarrage);
}

//----------------------------------------------------------------------
//!\brief           Gestion du mode stop
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int trt_mode_attente(void){
	syslog(LOG_DEBUG, "--trt_mode_attente :  On est en mode ATTENTE");
	
	//on récupère la température intérieure actuelle
	stDonnees.temperature_int = get_temperature();
	

	//si le mode précédent était différent du mode attente
	if(stDonnees.mode_prec != MODE_ATTENTE){
		//on envoie le message STOP au poele
		envoie_msg_poele(POELE_STOP);
		//on passe en mode ATTENTE
		change_mode(MODE_ATTENTE);
		//on sort
		return(0);
	}else{ //sinon

		//on cherche la prochaine consigne de chauffage de la journée	
		//si la consigne existe
		if( chercher_consigne_3h() == TRUE){

			// bon alors, maintenant le but est de calculer QUAND il faut démarrer le poele pour atteindre la consigne demandée à l'heure demandée.
			//on commence par calculer le temps qu'il va falloir pour passer de température actuelle à la température de consigne, en fonction des paramètres de la maison et du chauffage
			
			// on récupère la consigne, on va la cherche dans la BDD au cas où l'utilisateur (ou la domotique) l'aurait changé entre temps
			//stDonnees.temperature_consigne = get_consigne(); -> déplacé dans chercher_consigne_3h()
			
			// on calcule la consigne à appliquer en fonction de la T° ext et T° int. Ce calcul n'est fait qu'une seule fois lorsqu'on bascule en mode ANTICIPE
			stDonnees.consigne_anticipe = calcul_consigne_anticipe();
			
			// Si il faut chauffer... ben oui si il fait plus chaud que la consigne, pas besoin de chauffer!
			if(stDonnees.temperature_consigne > stDonnees.temperature_int ){
				// on calcule le temps en secondes pour atteindre la consigne , ATTENTION -> possible division par 0 !
				stDonnees.delta_time = (int)((( stDonnees.temperature_consigne - stDonnees.temperature_int ) / ( stDonnees.consigne_anticipe * COEF_THERM_MAISON / 100 ) ) + INERTIE_MAISON ) * 60; // *60 pour convertir en secondes (unité UNIXTIME)
				syslog(LOG_DEBUG, "  delta_time calcule pour atteindre la consigne : %d secondes soit %d minutes", stDonnees.delta_time, stDonnees.delta_time / 60);
				
				// on compare l'heure de consigne avec l'heure actuelle + le delta temps ... pour savoir si il faut allumer le poele, ou si on attend encore...
				unixtime_to_date(stDonnees.seconds);
				syslog(LOG_DEBUG, "  heure courante (UNIX time) : %d secondes - %s", stDonnees.seconds, datestring);
				
				//on récupère l'heure de cette consigne au format UNIX_TIME
				stDonnees.heure_debut_consigne  = get_heure_consigne();	
				
				stDonnees.heure_demarrage = stDonnees.heure_debut_consigne - stDonnees.delta_time;
				unixtime_to_date(stDonnees.heure_demarrage);
				syslog(LOG_DEBUG, "  heure de demarrage theorique du poele (UNIX time) (h_consigne - delta_time): %d secondes  - %s", stDonnees.heure_demarrage, datestring);
				
				//si l'heure courante est supérieure à l'heure de consigne - le delta), en clair, si il c'est l'heure de démarrer le poele... on passe en mode ANTICIPE
				if (stDonnees.seconds > stDonnees.heure_demarrage){
					//alors on passe en mode ANTICIPE
					change_mode(MODE_ANTICIPE);
					//on sauvegarde l'heure de début de consigne , le fonction get_etat() permet de restituer cette données dans stDonnees.heure_debut_consigne
					sauve_heure_debut_consigne(stDonnees.heure_debut_consigne);
					// Memo : la consigne appliquée pendant la phase ANTICIPE sera celle calculee par calcul_consigne_anticipe() , cf. au dessus
				}
			}
			// sinon on ne fait rien, pas besoin de chauffer quoi...
			else{
				syslog(LOG_DEBUG, "  On reste en mode ATTENTE car ce n'est pas le moment ou il fait assez chaud...");
				//on envoie le message STOP au poele
				envoie_msg_poele(POELE_STOP);
			}
		}
		//CAS PARTICULIER : La température baisse pendant la phase d'attente alors que le créneau de chauffe est en cours , l'anticipation n'est lancée !
		//on vérifie si nous somme dans un créneau de chauffe
		else if(chercher_consigne_now() == TRUE){
			//si on est dans un créneau de chauffe, on se magne et on allume le poêle si il fait froid 
			// on récupère la consigne, on va la cherche dans la BDD au cas où l'utilisateur (ou la domotique) l'aurait changé entre temps
			//stDonnees.temperature_consigne = get_consigne();  -> déplacé dans la fonctione chercher_consigne_now 
			
			// on calcule la consigne à appliquer en fonction de la T° ext et T° int. Ce calcul n'est fait qu'une seule fois lorsqu'on bascule en mode ANTICIPE
			stDonnees.consigne_anticipe = calcul_consigne_anticipe();

			
			//si la température intérieure est bien < à la consigne, on passe dans le mode ANTICIPE pour amorcer la phase de démarrage du poele
			if(stDonnees.temperature_int < stDonnees.temperature_consigne){
				syslog(LOG_DEBUG, "  On passe en mode ANTICIPE car la T° amb < T° consigne");
				//alors on passe en mode ANTICIPE
				change_mode(MODE_ANTICIPE);
				// Memo : la consigne appliquée pendant la phase ANTICIPE sera celle calculee par calcul_consigne_anticipe() , cf. au dessus
			}
			else{
				// il fait assz chaud 
				syslog(LOG_DEBUG, "  On reste en mode ATTENTE , il fait assez chaud...");
				//on envoie le message STOP au poele
				envoie_msg_poele(POELE_STOP);
			}
		}
		else{
			// condition ajoutée pour envoyer STOP à chaque fois pour éviter les ratés
			//on envoie le message STOP au poele
			envoie_msg_poele(POELE_STOP);
		}
		// s'il n'y a pas d'erreurs, tout est ok, on le dit
		return(0);
	}
}
//----------------------------------------------------------------------
//!\brief           Gestion du mode anticipe. Le but de ce mode est de lancer le poêle, on se contente de surveiller que la température ne dépasse pas la consigne OU passer en régulation de température si le créneau de chauffe est atteint.
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int trt_mode_anticipe(void){
	syslog(LOG_DEBUG, "--trt_mode_anticipe, On est en mode ANTICIPE");

	//on récupère la température intérieure actuelle
	stDonnees.temperature_int = get_temperature();
	// on récupère la consigne, on va la cherche dans la BDD au cas où l'utilisateur (ou la domotique) l'aurait changé entre temps
	stDonnees.temperature_consigne = get_consigne();
	
	//on récupère l'heure de cette consigne au format UNIX_TIME -> déjà dans la structure stDonnees , cf fonction get_etat() lancée dans le main
	// on affiche l'heure courante
	unixtime_to_date(stDonnees.seconds);
	syslog(LOG_DEBUG, "  Heure courante (UNIX time) : %d secondes - %s", stDonnees.seconds, datestring);
	
	
	// on calcule les paramètres du PID pour amorcer la régulation, n'est utile que si l'on passe ensuite en mode CHAUFFE -> permet de lancer le PID avec les bons coef
	init_PID(stDonnees.temperature_consigne, stDonnees.temperature_int);

	//si l'heure courante est supérieure à l'heure de consigne , alors on passe en mode régulation de température
	if(stDonnees.seconds >= stDonnees.heure_debut_consigne ){
		syslog(LOG_DEBUG, "Heure courante a dépasse l'heure de consigne, on change de mode");
		//on passe en mode CHAUFFE
		change_mode(MODE_CHAUFFE);
	//si la température intérieure dépasse la température de consigne, on passe en régulation
	}else if(stDonnees.temperature_consigne <= stDonnees.temperature_int ){
		syslog(LOG_DEBUG, "T° de consigne < température intérieure, on change de mode");
		//alors on passe en mode CHAUFFE
		change_mode(MODE_CHAUFFE);
	}else{
		//on reste en mode ANTICIPE
		//on applique la consigne anticipee
		stDonnees.puissance_chauffe = stDonnees.consigne_anticipe;
		syslog(LOG_DEBUG, " On reste en mode ANTICIPE, stDonnees.puissance_chauffe = %d", stDonnees.puissance_chauffe);
		//on envoie le message avec la consigne par défaut du poele
		envoie_msg_poele(stDonnees.puissance_chauffe);
	}
		
	// s'il n'y a pas d'erreurs, tout est ok, on le dit
	return(0);
}
//----------------------------------------------------------------------
//!\brief           Récupération du mode chauffe
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int trt_mode_chauffe(void){
	syslog(LOG_DEBUG, "--trt_mode_chauffe, On est en mode CHAUFFE");
	
	//on récupère la température intérieure actuelle
	stDonnees.temperature_int = get_temperature();
	// on récupère la consigne, on va la cherche dans la BDD au cas où l'utilisateur (ou la domotique) l'aurait changé entre temps
	stDonnees.temperature_consigne = get_consigne();
	//on récupère l'heure de cette consigne au format UNIX_TIME
	stDonnees.heure_fin_consigne  = get_heure_consigne();
	
	// on affiche l'heure courante
	unixtime_to_date(stDonnees.seconds);
	syslog(LOG_DEBUG, "heure courante (UNIX time) : %d secondes - %s", stDonnees.seconds, datestring);
	
	//si l'heure courante est supérieure à l'heure de fin consigne , alors on passe en mode ATTENTE car il n'y a plus besoin de chauffer
	if(stDonnees.seconds >= stDonnees.heure_fin_consigne ){
		syslog(LOG_DEBUG, "On a depasse l'heure de consigne");
		fflush(stdout);
		//on passe en mode ATTENTE
		change_mode(MODE_ATTENTE);
		// nota : on envoie pas l'info OFF au poele, ça sera automatique à la prochaine boucle de traitement (au cas ou il ne faudrait pas éteindre le poele...)
	}else{
		// ICI trouver des conditions pour arrêter le poêle dans certaines conditions :
		//	-  puissance chauffe =0%  ET (T° int - T Consigne) > 2°C ?
		//	- Puissance solaire > xxx
		
		if( (stDonnees.temperature_int - stDonnees.temperature_consigne) >= DELTA_TEMP_MAX ){
			syslog(LOG_DEBUG, "T° int > T° consigne + %f °C , on passe en mode ATTENTE ! ", DELTA_TEMP_MAX);
			// on passe en mode ATTENTE, au prochain cycle la consigne d'extinction du poele sera envoyée
			change_mode(MODE_ATTENTE);
		}else{
			//on calcule la consigne avec le PID
			stDonnees.puissance_chauffe = PID(stDonnees.temperature_consigne, stDonnees.temperature_int);
			//on envoie le message avec la consigne calculée en sortie du PID
			envoie_msg_poele(stDonnees.puissance_chauffe);			
		}	
	}
	// s'il n'y a pas d'erreurs, tout est ok, on le dit
	return(0);
}

//----------------------------------------------------------------------
//!\brief		calcul de la consigne à appliquer en mode ANTICIPE
//!\return		puissance de chauffe
//----------------------------------------------------------------------
int calcul_consigne_anticipe(void){
	syslog(LOG_DEBUG, "--calcul_consigne_anticipe");
	float temperature;

	// Preparation de la requete MySQL
	//sprintf(query, "SELECT ana1 FROM analog2 ORDER BY date_time DESC limit 0,1");
	sprintf(query, "SELECT ana1,`date_time` FROM analog1 WHERE date_time >= SUBTIME( NOW( ) ,  '0:45:00' ) ORDER BY date_time DESC limit 0,1");

	if (mysql_query(conn, query)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
    }
	
	//on récupère les valeurs
	result = mysql_use_result(conn);	

	if (row = mysql_fetch_row(result)){
		//on converti la chaine en float
		temperature = atof(row[0]);
	}else {
		syslog(LOG_DEBUG, "ERREUR : PAS DE TEMPERATURE INTERIEURE RECUPEREE");
		// on retourne la consigne par défaut
		stDonnees.puissance_chauffe = P_POELE_ANTICIPE;
		syslog(LOG_DEBUG, "  on force la consigne de chauffe a : %d", P_POELE_ANTICIPE);
		// on retourne avec une erreur
		return(1);
	}
	
	// on libère la mémoire
	mysql_free_result(result);
	
	// on calcul la consigne avec la formule magique
	stDonnees.puissance_chauffe = (int) (P_POELE_OFFSET + (stDonnees.temperature_consigne - stDonnees.temperature_int) * (stDonnees.temperature_consigne - temperature));
	syslog(LOG_DEBUG, "  calcul chauffe brute calculee : ( %0.2f - %0.2f ) * ( %0.2f - %0.2f )", stDonnees.temperature_consigne, stDonnees.temperature_int, stDonnees.temperature_consigne, temperature);
	syslog(LOG_DEBUG, "  puissance de chauffe brute calculee :%d", stDonnees.puissance_chauffe);
	//on borne la consigne entre 0 et 100%
	if(stDonnees.puissance_chauffe > 100)
		stDonnees.puissance_chauffe = 100;
	else if(stDonnees.puissance_chauffe <= 0)
		// on met 1% pour eviter une division par zero dans les calculs ulterieurs
		stDonnees.puissance_chauffe = 1;
	
	
	// sauvegarde de la valeur dans la BDD
	// Preparation de la requete MySQL
	// on met à jour la BDD sur le mode
	sprintf(query, "UPDATE `domotique`.`chauffage_statut` SET `consigne_anticipe` = '%d' WHERE `chauffage_statut`.`id` =1;", stDonnees.puissance_chauffe);
	
	// envoi de la requete
	// printf("\nEnvoi de la requete : %s", query);
	if (mysql_query(conn, query)) {
		// si la requete echoue on retourne 1
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
	}
	
	syslog(LOG_DEBUG, "  puissance de chauffe bornee entre 1-100 :%d", stDonnees.puissance_chauffe);
	syslog(LOG_DEBUG, "  puissance %d ecrit dans la table chauffage_statut.", stDonnees.puissance_chauffe);
	return(stDonnees.puissance_chauffe);
}

//----------------------------------------------------------------------
//!\brief           Cherche dans le planning si une consigne de chauffe va arriver
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int chercher_consigne_3h(void){
	syslog(LOG_DEBUG, "  --chercher_consigne_3h, On cherche s'il existe une consigne dans les 3h...");
	
	// on va chercher la prochaine consigne du jour, on regarde si une consigne va arriver dans les 3 prochaines heure
	//SELECT UNIX_TIMESTAMP( addtime( `date` , `heure_debut` ) ) FROM `calendrier_jour_en_cours` WHERE UNIX_TIMESTAMP( addtime( `date` , `heure_debut` ) ) <= UNIX_TIMESTAMP( ADDTIME( NOW( ) , '04:00:00' ) ) AND UNIX_TIMESTAMP( addtime( `date` , `heure_debut` ) ) > UNIX_TIMESTAMP( NOW( ) ) ORDER BY heure_debut ASC
	// Preparation de la requete MySQL
	// on ne récupère que les infos du jour en cours
		
	// avec table calendrier_30min
	// SELECT  UNIX_TIMESTAMP( addtime( `date` , `heure_debut`) ), addtime( `date` , `heure_debut`), `temperature`
	// FROM `calendrier_30min` 
	// WHERE addtime( `date` , `heure_debut` )  <= ADDTIME( NOW( ) , '04:00:00' ) 
	// AND addtime( `date` , `heure_debut` )  >  NOW( )  
	// AND `temperature` != 0.0 
	// ORDER BY heure_debut ASC 
	// LIMIT 1
	
	// avec table calendrier_jour_en_cours (obsolète)
	//SELECT UNIX_TIMESTAMP( addtime( `date` , `heure_debut` ) ) FROM `calendrier_jour_en_cours` WHERE UNIX_TIMESTAMP( addtime( `date` , `heure_debut` ) ) <= UNIX_TIMESTAMP( ADDTIME( NOW( ) , '04:00:00' ) ) AND UNIX_TIMESTAMP( addtime( `date` , `heure_debut` ) ) > UNIX_TIMESTAMP( NOW( ) ) ORDER BY heure_debut ASC
	sprintf(query, "SELECT  `temperature`, UNIX_TIMESTAMP( addtime( `date` , `heure_debut`) ), addtime( `date` , `heure_debut`) FROM `calendrier_30min` WHERE addtime( `date` , `heure_debut` )  BETWEEN NOW() AND  ADDTIME( NOW( ) , '04:00:00' ) AND `temperature` != 0.0 ORDER BY ADDTIME( `date` , `heure_debut` ) ASC LIMIT 1");

	// envoi de la requete
	// printf("\nEnvoi de la requete : %s", query);
	if (mysql_query(conn, query)) {
		// si la requete echoue on retourne 1
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
	}

	// la requête s'est bien passée, on rend le jeu de résultats disponible via le pointeur result
	//printf("\nRecuperation des donnees");
	result = mysql_use_result(conn);

	//si une ligne est retournée
	if (row = mysql_fetch_row(result)){
		//c'est qu'il y a une requête ... faut-il vérifier son intégrite????
		
		// on en profite pour récupérer la consigne de température
		//on converti la chaine en float
		stDonnees.temperature_consigne = atof(row[0]);		
		// on libère la requête
		mysql_free_result(result);
		syslog(LOG_DEBUG, "    Il existe une consigne : %2.1f", stDonnees.temperature_consigne);
		// on retourne 1 pour signaler qu'il existe une consigne
		return(1);
	// si la requete ne retourne aucun résultat
	}else{
		// c'est qu'il n'y a pas de consigne de chauffe en vue !
		syslog(LOG_DEBUG, "    Il n'y a pas de consigne dans les 3 prochaines heures.");
		// on retourne 0
		// on libère la requête
		mysql_free_result(result);
		return(0);
	}
	
	// si on passe ici... gros bug, on retourne une erreur
	return(0);
}
//----------------------------------------------------------------------
//!\brief           Cherche dans le planning si une consigne est en cours
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int chercher_consigne_now(void){
	syslog(LOG_DEBUG, "--chercher_consigne_now, On cherche s'il existe une consigne en cours...");
	
	// on va chercher la prochaine consigne en cours (obsolète, utilise table calendrier_jour_en_cours)
	// on ne récupère que les infos du jour en cours
	//SELECT addtime( `date` , `heure_debut` ) FROM `calendrier_jour_en_cours` WHERE UNIX_TIMESTAMP( addtime( `date` , `heure_debut` ) ) <= UNIX_TIMESTAMP( NOW( ) ) AND UNIX_TIMESTAMP( addtime( `date` , `heure_fin` ) ) > UNIX_TIMESTAMP( NOW( ) ) ORDER BY heure_debut ASC LIMIT 0 , 30
	
	// requête avec table calendrier_30min
	// SELECT addtime( `date` , `heure_debut` ) 
	// FROM `calendrier_30min` 
	// WHERE addtime( `date` , `heure_debut` ) <= NOW( ) 
	// AND addtime( `date` , `heure_fin` ) > NOW( ) 
	// AND `temperature` != 0.0 
	// ORDER BY heure_debut ASC 
	// LIMIT 1
	
	// Preparation de la requete MySQL
	sprintf(query, "SELECT `temperature`, addtime( `date` , `heure_debut` )  FROM `calendrier_30min` WHERE Now( ) BETWEEN addtime( `date` , `heure_debut` ) AND addtime( `date` , `heure_fin` ) AND `temperature` != 0.0 ORDER BY ADDTIME( `date` , `heure_debut` ) ASC LIMIT 1");
	// envoi de la requete
	// printf("\nEnvoi de la requete : %s", query);
	if (mysql_query(conn, query)) {
		// si la requete echoue on retourne 1
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
	}

	// la requête s'est bien passée, on rend le jeu de résultats disponible via le pointeur result
	//printf("\nRecuperation des donnees");
	result = mysql_use_result(conn);

	//si une ligne est retournée
	if (row = mysql_fetch_row(result)){
		//c'est qu'il y a une requête ... faut-il vérifier son intégrite????
		syslog(LOG_DEBUG, "Il existe une consigne en cours alors qu'on est en mode ATTENTE");
		// on en profite pour récupérer la consigne de température
		//on converti la chaine en float
		stDonnees.temperature_consigne = atof(row[0]);
		// on libère la requête
		mysql_free_result(result);
		// on retourne 1 pour signaler qu'il existe une consigne
		return(1);
	// si la requete ne retourne aucun résultat
	}else{
		// c'est qu'il n'y a pas de consigne de chauffe en vue !
		syslog(LOG_DEBUG, "  Il n'y a pas de consigne en cours.");
		// on retourne 0
		// on libère la requête
		mysql_free_result(result);
		return(0);
	}
	// si on passe ici... gros bug, on retourne une erreur
	return(1);
}

//----------------------------------------------------------------------
//!\brief           Ecriture du nouvel etat dans la BDD
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int change_etat(int etat){
	syslog(LOG_DEBUG, "--change_etat");
	// Preparation de la requete MySQL
	//UPDATE `domotique`.`chauffage_statut` SET `etat` = '1',`etat_prec` = '1' WHERE `chauffage_statut`.`id` =1;
	sprintf(query, "UPDATE `domotique`.`chauffage_statut` SET `etat` = '%d',`etat_prec` = '%d' WHERE `chauffage_statut`.`id` =1;", etat, stDonnees.etat);

	// envoi de la requete
	// printf("\nEnvoi de la requete : %s", query);
	if (mysql_query(conn, query)) {
		// si la requete echoue on retourne 1
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
	}
	syslog(LOG_DEBUG, "Mode %d ecrit dans la base.", etat);
	// s'il n'y a pas d'erreurs, tout est ok, on le dit
	return(0);
}

//----------------------------------------------------------------------
//!\brief           Ecriture du nouveau mode dans la BDD
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int change_mode(int mode){
	syslog(LOG_DEBUG, "--change_mode, On change de mode");
	//on affecte le mode en cours au mode demandé
	stDonnees.mode_prec = stDonnees.mode;
	stDonnees.mode = mode;

	// Preparation de la requete MySQL
	// on met à jour la BDD sur le mode
	sprintf(query, "UPDATE `domotique`.`chauffage_statut` SET `mode` = '%d',`mode_prec` = '%d' WHERE `chauffage_statut`.`id` =1;", stDonnees.mode, stDonnees.mode_prec);
	
	// envoi de la requete
	// printf("\nEnvoi de la requete : %s", query);
	if (mysql_query(conn, query)) {
		// si la requete echoue on retourne 1
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
	}

	syslog(LOG_DEBUG, " Mode %d ecrit dans la base.", mode);
	// s'il n'y a pas d'erreurs, tout est ok, on le dit
	return(0);
}

//----------------------------------------------------------------------
//!\brief           Récupération de la température intérieure
//!\return        température intérieure actuelle (float)
//----------------------------------------------------------------------
float get_temperature(void) {
	syslog(LOG_DEBUG, "--get_temperature");
	float temperature;

	// Preparation de la requete MySQL
	//sprintf(query, "SELECT ana1 FROM analog2 ORDER BY date_time DESC limit 0,1");
	sprintf(query, "SELECT ana1 FROM analog2 ORDER BY date_time DESC limit 0,1");

	if (mysql_query(conn, query)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
    }
	
	//on récupère les valeurs
	result = mysql_use_result(conn);	

	if (row = mysql_fetch_row(result)){
		//on converti la chaine en float
		temperature = atof(row[0]);
	}else {
		syslog(LOG_DEBUG, "ERREUR : PAS DE TEMPERATURE INTERIEURE RECUPEREE");
		temperature = 17.0;
	}
	
	syslog(LOG_DEBUG, "  temperature interieure actuelle :%.2f", temperature);
		
	// on ligère la mémoire
	mysql_free_result(result);
	return(temperature);
}

//----------------------------------------------------------------------
//!\brief           Récupération de la température de consigne
//!\return        température de consigne (float)
//----------------------------------------------------------------------
float get_consigne(void){
	syslog(LOG_DEBUG, "--get_consigne");
	float temperature_consigne;

	// ATTENTION : en fonction du mode où l'on se trouve, la consigne est à récupérer de différentes façons
	// on prépare donc une requete différente en fonction des modes
	// Preparation de la requete MySQL
	
	switch(stDonnees.mode){
		case MODE_ANTICIPE:
		case MODE_ATTENTE:
			// la consigne se trouve dans le futur
			//version avec table calendrier_jour_en_cours (obsolète))
			//SELECT UNIX_TIMESTAMP( ADDTIME( `date` , `heure_debut` ) ) FROM `calendrier_jour_en_cours` WHERE UNIX_TIMESTAMP( addtime( `date` , `heure_debut` ) ) <= UNIX_TIMESTAMP( ADDTIME( NOW( ) , '04:00:00' ) ) AND UNIX_TIMESTAMP( addtime( `date` , `heure_debut` ) ) > UNIX_TIMESTAMP( NOW( ) ) ORDER BY heure_debut ASC
			// avec table calendrier_30min, la requete change : il faut borner la recherche dans le temps et enlever les créneau sans consignes (température = 0.0)
				// SELECT `temperature` , ADDTIME( `date` , `heure_debut` ) 
				// FROM `calendrier_30min` 
				// WHERE ADDTIME( `date` , `heure_debut` ) <= ADDTIME( NOW( ) , '04:00:00' ) 
				// AND ADDTIME( `date` , `heure_debut` ) >= NOW()
				// AND `temperature` != 0.0
				// ORDER BY ADDTIME( `date` , `heure_debut` )  DESC 
				// LIMIT 1
			sprintf(query, "SELECT `temperature` , ADDTIME( `date` , `heure_debut` ) FROM `calendrier_30min` WHERE ADDTIME( `date` , `heure_debut` ) BETWEEN NOW() AND ADDTIME( NOW( ) , '04:00:00' ) AND `temperature` != 0.0	ORDER BY ADDTIME( `date` , `heure_debut` )  DESC LIMIT 1");
			break;
		// on a un créneau à chauffer qui arrive, on estime combien de temps avant il faut démarrer
		case MODE_CHAUFFE:
			// la consigne se trouve dans le présent
			//version avec table calendrier_jour_en_cours (obsolète))
			//SELECT `temperature` FROM `calendrier_jour_en_cours` WHERE UNIX_TIMESTAMP( addtime( `date` , `heure_debut` ) ) <= UNIX_TIMESTAMP(NOW( ))  ORDER BY heure_debut DESC
			//version avec table calendrier_30min
				// SELECT `temperature` , ADDTIME( `date` , `heure_debut` ) 
				// FROM `calendrier_30min` 
				// WHERE ADDTIME( `date` , `heure_debut` ) <= NOW( ) 
				// AND `temperature` != 0.0
				// ORDER BY ADDTIME( `date` , `heure_debut` ) DESC 
				// LIMIT 1
			sprintf(query, "SELECT `temperature` , ADDTIME( `date` , `heure_debut` ) FROM `calendrier_30min` WHERE NOW() >= ADDTIME( `date` , `heure_debut` ) AND `temperature` != 0.0	ORDER BY ADDTIME( `date` , `heure_debut` ) DESC 	LIMIT 1");
			break;
		// ça y est, ça chauffe... on gère la puissance pour le confort des habitants
		case MODE_ERREUR:
			// si on passe là, c'est vraiment la grosse m..rde!
			return(1);
			break;
	}	
	
	if (mysql_query(conn, query)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
    }
	
	result = mysql_use_result(conn);	
	// on lit la ligne demandée
	if (row = mysql_fetch_row(result)){
		//on converti la chaine en float
		temperature_consigne = atof(row[0]);
	}else {
		syslog(LOG_DEBUG, "ERREUR : PAS DE TEMPERATURE DE CONSIGNE RECUPEREE !");
		temperature_consigne = 20.0;
	}
	
	syslog(LOG_DEBUG, "temperature consigne recuperee :%.2f", temperature_consigne);
	
	// on libère la mémoire
	mysql_free_result(result);
	
	return(temperature_consigne);
}
//----------------------------------------------------------------------
//!\brief           Récupération de l'heure de consigne
//!\return        heure de consigne en UNIX TIme
//----------------------------------------------------------------------
int get_heure_consigne(void){
	syslog(LOG_DEBUG, "--get_heure_consigne");
	int heure_consigne;
	
	// ATTENTION : en fonction du mode où l'on se trouve, l'heure de consigne est à récupérer de différentes façons
	// on prépare donc une requete différente en fonction des modes
	// Preparation de la requete MySQL
	
	switch(stDonnees.mode){
		case MODE_ATTENTE:
		case MODE_ANTICIPE: // en théorie, dans le mode ANTICIPE, on ne passe pas ici ! l'heure de debut de consigne est récupérée dans la table status dans la fonction get_etat()
			// la consigne se trouve dans le futur
			//requete avec table calendrier_jour_en_cours (obsolète)
			//SELECT UNIX_TIMESTAMP( addtime( `date` , `heure_debut` ) ),`heure_debut`  FROM `calendrier_jour_en_cours` WHERE UNIX_TIMESTAMP( addtime( `date` , `heure_debut` ) ) <= UNIX_TIMESTAMP( ADDTIME( NOW( ) , '04:00:00' ) )  ORDER BY heure_debut DESC
			//requet avec table calendrier_30min
			// SELECT UNIX_TIMESTAMP( addtime( `date` , `heure_debut` ) ), addtime( `date` , `heure_debut` )  
			// FROM `calendrier_30min` 
			// WHERE addtime( `date` , `heure_debut` ) <= ADDTIME( NOW( ) , '04:00:00' ) 
			// AND  addtime( `date` , `heure_debut` ) >= NOW()
			// AND `temperature` != 0.0
			// ORDER BY ADDTIME( `date` , `heure_debut` )  DESC 
			// LIMIT 1
			sprintf(query, "SELECT UNIX_TIMESTAMP( addtime( `date` , `heure_debut` ) ), ADDTIME( `date` , `heure_debut` ) FROM `calendrier_30min` WHERE ADDTIME( `date` , `heure_debut` ) <= ADDTIME( NOW( ) , '04:00:00' ) AND  addtime( `date` , `heure_debut` ) >= NOW() AND `temperature` != 0.0 ORDER BY ADDTIME( `date` , `heure_debut` )  ASC LIMIT 1");
			break;
		// on a un créneau à chauffer qui arrive, on estime combien de temps avant il faut démarrer
		case MODE_CHAUFFE:
			// on cherche l'heure de fon de consigne!
			//SELECT UNIX_TIMESTAMP( addtime( `date` , `heure_fin` ) ) FROM `calendrier_jour_en_cours` WHERE UNIX_TIMESTAMP( addtime( `date` , `heure_debut` ) ) <= UNIX_TIMESTAMP(  NOW( )  ) ORDER BY heure_debut DESC
			//version avec table calendrier_30min
			// SELECT UNIX_TIMESTAMP( addtime( `date` , `heure_fin` ) ) , ADDTIME( `date` , `heure_fin` )
			// FROM `calendrier_30min` 
			// WHERE ADDTIME( `date` , `heure_fin` ) <= ADDTIME( NOW( ) , '01:30:00')
			// AND ADDTIME( `date` , `heure_fin` ) >=  NOW( )
			// AND `temperature` != 0.0
			// ORDER BY ADDTIME( `date` , `heure_fin` ) DESC 
			// LIMIT 1
			sprintf(query, "SELECT UNIX_TIMESTAMP( addtime( `date` , `heure_fin` ) ) , ADDTIME( `date` , `heure_fin` ) FROM `calendrier_30min` WHERE ADDTIME( `date` , `heure_fin` ) <= ADDTIME( NOW( ) , '01:30:00')AND ADDTIME( `date` , `heure_fin` ) >=  NOW( )AND `temperature` != 0.0ORDER BY ADDTIME( `date` , `heure_fin` ) DESC LIMIT 1");
			break;
		// ça y est, ça chauffe... on gère la puissance pour le confort des habitants
		case MODE_ERREUR:
			// si on passe là, c'est vraiment la grosse m..rde!
			return(1);
			break;
	}

	if (mysql_query(conn, query)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
    }
		
	result = mysql_use_result(conn);
	
	//si la requete a retourné une ligne non nulle
	// on lit la ligne demandée
	if (row = mysql_fetch_row(result)){
		//on converti la chaine en float
		heure_consigne = atoi(row[0]);
		unixtime_to_date(heure_consigne);
		syslog(LOG_DEBUG, "heure de consigne recuperee (UNIX time):%d - %s", heure_consigne, datestring);
	}else{
		// euh... je réfléchis
		syslog(LOG_DEBUG, "ERREUR : PAS D'HEURE DE CONSIGNE TROUVEE");
		heure_consigne = 0; 
	}

	// on ligère la mémoire
	mysql_free_result(result);
	
	return(heure_consigne);
}

//----------------------------------------------------------------------
//!\brief           fonction PID
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
/*
PID : fonction qui effectue un asservissement PID
Ci : Consigne initiale (ce qu'on veut qu'il fasse)
stPID_e : erreur entre la consigne initiale et la réalité
C : Consigne appliquée au moteur
R : Grandeur réelle mesurée (réalité)
*/
 
// float PID(float Ci,float R)
// {
	// float P,D;
	// float C;
	// stPID_Old_e = stPID_e;
	// stPID_e = Ci-R;

	// //Terme Proportionnel
	// P = stPID_e;
	// //Terme Integral
	// stPID_I = stPID_I+stPID_e;
	// //Terme D&eacute;riv&eacute;
	// D = stPID_e-stPID_Old_e;
	// // oops on a fini
	// C = Kp * P + Ki * stPID_I + Kd * D;
  
	// printf("Ci:%.2f - R:%.2f\n",Ci,R);
	// printf("P: %.2f  - stPID_I:%.2f - D:%.2f\n",P,stPID_I,D);
 
  // return C;
// }

float PID(float Ci,float mesure){
	syslog(LOG_DEBUG, "--PID()");
	float C, ecart, delta, deltaP, deltaI, deltaD, D, Consigne;
	
	// Récupération des données du PID
	if(recup_donnees_PID() == 1) {
		// erreur dans le log
		// prévoir un msg de log vers un fichier de log
		exit(1);
	}
	
	syslog(LOG_DEBUG, "Calcul consigne en cours");
	fflush(stdout);
	ecart = Ci - mesure;
	deltaP = Kp * (ecart - stPID_poele.ecart_prec);
	deltaI = Kp * ecart / Ti;
	D = (Kp * Td) / (Td + Kp * dt) * ( ecart - stPID_poele.ecart_prec + stPID_poele.D_prec / Kd) ;
	deltaD = D - stPID_poele.D_prec;
	delta = deltaP + deltaI + deltaD ;
	Consigne = stPID_poele.Consigne_prec + delta;
	//limitation de la consigne
	if(Consigne > 100){
		syslog(LOG_DEBUG, "Consigne > 100 (%.2f) , LIMITATION a 100 !",Consigne);
		Consigne = 100.0;
	}else if(Consigne < 0){
		syslog(LOG_DEBUG, "Consigne < 0 (%.2f) , LIMITATION a 00 !",Consigne);
		Consigne = 0.0;
	}
	stPID_poele.D_prec = D;
	stPID_poele.Consigne_prec = Consigne;
	stPID_poele.ecart_prec = ecart;
 
	syslog(LOG_DEBUG, "Ecart:%.2f - deltaP:%.2f - deltaI:%.2f - deltaD:%.2f",ecart,deltaP,deltaI,deltaD);
	syslog(LOG_DEBUG, "delta: %.2f  - Consigne:%.2f",delta,Consigne);
	// on sauve les variables du PID dans la BDD
	if(sauve_donnees_PID() == 1) {
		// erreur dans le log
		// prévoir un msg de log vers un fichier de log
		exit(1);
	}
  return Consigne;
}

//----------------------------------------------------------------------
//!\brief           Fonction qui récupère les variables du PID depuis la BDD
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int recup_donnees_PID(){

	syslog(LOG_DEBUG, "--recup_donnees_PID, Recuperation des donnees PID dans la BDD...");
	fflush(stdout);
	// Preparation de la requete MySQL
	// on ne récupère que les infos du jour en cours
	sprintf(query, "SELECT * FROM `chauffage_PID`");

	// envoi de la requete
	// printf("\nEnvoi de la requete : %s", query);
	if (mysql_query(conn, query)) {
		// si la requete echoue on retourne 1
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(-1);
	}

	// la requête s'est bien passée, on rend le jeu de résultats disponible via le pointeur result
	//printf("\nRecuperation des donnees");
	result = mysql_store_result(conn);

	// si la requete retourne un résultat
	if (row = mysql_fetch_row(result)){

		//création de la variable string qui va contenir le mode contenu dans la base
		// on converti le string en float
		stPID_poele.D_prec = atof(row[ROW_D_prec]);

		//création de la variable string qui va contenir le mode précédent contenu dans la base
		// on converti le string en float
		stPID_poele.ecart_prec = atof(row[ROW_ecart_prec]);

		//création de la variable string qui va contenir le mode précédent contenu dans la base
		// on converti le string en float
		stPID_poele.Consigne_prec = atof(row[ROW_Consigne_prec]);

		// on libère la requête
		mysql_free_result(result);
	
		syslog(LOG_DEBUG, "Donnees recuperees -> D_prec=%.2f , ecart_prec=%.2f , Consigne_prec=%.2f",stPID_poele.D_prec, stPID_poele.ecart_prec, stPID_poele.Consigne_prec);
	
	// si requete renvoi du vide
	}else{
	
		// probleme : aucune ligne retournée... MERDE! PAS NORMAL
		// on fixe des valeurs par défaut
		reset_PID();
	}
	
	// s'il n'y a pas d'erreurs, tout est ok, on le dit
	return(0);
}

//----------------------------------------------------------------------
//!\brief           Fonction qui sauve les variables du PID dans la BDD
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int sauve_donnees_PID(){

	syslog(LOG_DEBUG, "--sauve_donnees_PID, Sauvegarde des donnees PID dans la BDD en cours...");
	
	// Preparation de la requete MySQL
	// on ne stocke que les données de la structure
	sprintf(query,"UPDATE `domotique`.`chauffage_PID` SET `ecart_prec` = '%f',`D_prec` = '%f' ,`Consigne_prec` = '%f'WHERE `chauffage_PID`.`id` =1;", stPID_poele.ecart_prec, stPID_poele.D_prec, stPID_poele.Consigne_prec);	// envoi de la requete
	//printf("\nEnvoi de la requete : %s", query);
	if (mysql_query(conn, query)) {
		// si la requete echoue on retourne 1
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
	}
	syslog(LOG_DEBUG, "Donnees PID sauvees dans la bdd -> D_prec=%.2f , ecart_prec=%.2f , Consigne_prec=%.2f",stPID_poele.D_prec, stPID_poele.ecart_prec, stPID_poele.Consigne_prec);	
	// s'il n'y a pas d'erreurs, tout est ok, on le dit
	return(0);

}

//----------------------------------------------------------------------
//!\brief           Fonction qui réinitialise les varibles du PID
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int reset_PID(){
	syslog(LOG_DEBUG, "--reset_PID");
	stPID_poele.D_prec = 0.0;
	stPID_poele.ecart_prec = 0.0;
	stPID_poele.Consigne_prec = 0.0;
	sauve_donnees_PID();
	// s'il n'y a pas d'erreurs, tout est ok, on le dit
	return(0);
}

//----------------------------------------------------------------------
//!\brief           Fonction qui fixe la consigne PID quand on passe du mode ATTENTE au mode CHAUFFE
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int init_PID(float Ci,float mesure){
	syslog(LOG_DEBUG, "--init_PID");
	stPID_poele.D_prec = 0.0;
	stPID_poele.ecart_prec = Ci - mesure;
	stPID_poele.Consigne_prec = stDonnees.consigne_anticipe;
	sauve_donnees_PID();
	// s'il n'y a pas d'erreurs, tout est ok, on le dit
	return(0);
}

//----------------------------------------------------------------------
//!\brief           Fonction qui log la structure stDonnees dans la BDD pour mise au point
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
int log_donnees(void){
	syslog(LOG_DEBUG, "--log_donnees");
	// Preparation de la requete MySQL
	// on ne stocke que les données de la structure
	sprintf(query, "INSERT INTO `domotique`.`chauffage_log` (`id` ,`date_time` ,`etat` ,`etat_prec` ,`mode` ,`mode_prec` ,`consigne_utilisateur`, `heure_debut_consigne`, `heure_fin_consigne`,`heure_demarrage`, `temp_int`, `temp_consigne`, `delta_time` ,`puissance` )VALUES (NULL , FROM_UNIXTIME(%d) ,'%d' ,'%d', '%d', '%d', '%d', FROM_UNIXTIME(%d), FROM_UNIXTIME(%d), FROM_UNIXTIME(%d), '%.2f', '%.2f', '%d', '%d');", stDonnees.seconds, stDonnees.etat, stDonnees.etat_prec, stDonnees.mode, stDonnees.mode_prec, stDonnees.consigne_utilisateur, stDonnees.heure_debut_consigne, stDonnees.heure_fin_consigne, stDonnees.heure_demarrage,stDonnees.temperature_int, stDonnees.temperature_consigne, stDonnees.delta_time, stDonnees.puissance_chauffe);	// envoi de la requete
	//printf("\nEnvoi de la requete : %s", query);
	if (mysql_query(conn, query)) {
		// si la requete echoue on retourne 1
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(1);
	}

	syslog(LOG_DEBUG, "  Message de log ecrit dans la bdd");
	// s'il n'y a pas d'erreurs, tout est ok, on le dit
	return(0);
}

//----------------------------------------------------------------------
//!\brief           Fonction de simulation MODE TEST UNIQUEMENT
//!\return        1 si erreur, 0 sinon
//----------------------------------------------------------------------
float simumaison(float consigne_,float temperature){
	// on simule la montée en température de la maison en fonction de la consigne et de la température actulle, et ce sur une période de 5 minutes.
	float CoefChauffeMaison = 0.03762/0.95 ; //en °C/min/%
	temperature = temperature + (CoefChauffeMaison * consigne_/100 * dt );
 	syslog(LOG_DEBUG, "temperature apres simu: %.2f \n",temperature);
  return temperature;
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

	// init du fichier log (systeme)
	// LOG_CONS, on écrit sur la console en cas de défaillance de syslog
	openlog(log_name, LOG_CONS, LOG_USER);
	syslog(LOG_DEBUG, "***************************************************************");
	syslog(LOG_DEBUG, "Demarrage REGUL_TEMP.C, logiciel de calcul de consigne du poele");
	printf("Pour les messages de DEBUG, voir syslog : \\var\\log\\user.log \n");

	//initialisation BDD
	if(init_mysql() == 1) {
		// erreur de liaison avec la bdd
		syslog(LOG_DEBUG, "Erreur de connexion a MySQL");
		exit(1);
	}
	
	// Pour mémo lors de la lecture des logs
	syslog(LOG_DEBUG, "ETAT -> 0: ERREUR    1: OFF      2: ON           3: AUTO");
	syslog(LOG_DEBUG, "MODE -> 0: ERREUR    1: ATTENTE  2: ANTICIPE     3: CHAUFFE");
	
	//lecture de la date/heure courante
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
	
	//***************************************************************************
	//* PROGRAMME PRINCIPAL 
	//***************************************************************************
	
	// DEBUG
	//on récupère les données de température
	//float temperature = get_temperature();
	//float temperature = 17.0;
	//Pour le chauffage
	//float consigne_poele = 0.0;
	
	//on récupère l'état en cours
	get_etat();
	// en fonction de la machine d'état
	switch(stDonnees.etat){
	// on est  à l'arrêt, on va chercher si le poele va devoir chauffer...
	case ETAT_OFF:
		syslog(LOG_DEBUG, "On est en mode OFF");
	case ETAT_ERREUR:  // si pas d'état reconnu, on passe en état OFF
		trt_ETAT_OFF();
		break;
	// on a un créneau à chauffer qui arrive, on estime combien de temps avant il faut démarrer
	case ETAT_ON:
		trt_ETAT_ON();                    
		break;
	// ça y est, ça chauffe... on gère la puissance pour le confort des habitants
	case ETAT_AUTO:
		trt_ETAT_AUTO();
		break;
	}	
				      
	//avant de sortir, on log les données de la structure stDonnees pour la mise au pointeur
	if(log_donnees() == 1) {
		// erreur dans le log
		syslog(LOG_DEBUG, "log_donnees() : Erreur lors de l'enregistrement des donnees dans la bdd");
		exit(1);
	}
	
	// fermeture de MySQL
	mysql_close(conn);
	printf("\nEXIT \n\n");
	syslog(LOG_DEBUG, "EXIT NORMALY REGUL_TEMP.C");
	closelog();
	//exit(0);
}
