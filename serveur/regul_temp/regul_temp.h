//----------------------------------------------------------------------
//!\file    regul_temp.h
//!\date	25/12/2013
//!\author  Minbiocabanon
//----------------------------------------------------------------------

// prototypes des fonctions utilisées
void sigfun(int sig);
int init_mysql(void);
int envoie_msg_poele(int consigne_p);
int get_etat(void);
int trt_ETAT_OFF(void);
int trt_ETAT_ON(void);
int trt_ETAT_AUTO(void);
int get_mode(void);
int get_h_dem(void);
int trt_mode_attente(void);
int trt_mode_anticipe(void);
int trt_mode_chauffe(void);
int chercher_consigne_3h(void);
int calcul_consigne_anticipe(void);
int change_mode(int mode);
float get_temperature(void);
float get_consigne(void);
int get_heure_consigne(void);
float PID(float Ci,float mesure);
int sauve_donnees_PID(void);
int recup_donnees_PID(void);
int log_donnees(void);
float simumaison(float consigne_,float temperature);
int reset_PID(void);
int init_PID(float Ci,float mesure);
int sauve_heure_debut_consigne(unsigned int heure);
int unixtime_to_date(unsigned int unixstamp);


typedef struct stData stData;
struct stData{
	unsigned int seconds ; 				// Date/heure au format UNIXTIME
	unsigned int etat ; 				// Etat de la machine d'état, mode OFF par défaut
	unsigned int etat_prec ; 			// Etat précédent de la machine d'état,
	unsigned int mode ;  				// Mode de la machine d'état, mode STOP par défaut (état le plus stable.. hum hum)
	unsigned int mode_prec ;  			// Mode précédent de la machine d'état
	unsigned int consigne_utilisateur; 	// Consigne donnée par l'utilisateur à travers l'interface domotique. cas de l'état ON forcé.
	unsigned int heure_debut_consigne;	// Heure de début de consigne en UNIXTIME
	unsigned int heure_fin_consigne; 	// Heure de fin de consigne en UNIXTIME
	unsigned int heure_demarrage; 		// Heure de fin de démarrage théorique du poele
	float temperature_int;				// Temperature intérieure
	float temperature_consigne;			// 
	unsigned int consigne_anticipe;		// Consigne calculee pour la phase d'allumage anticipee du poele
	int delta_time;						// Temps à attendre avant la mise en route
	int puissance_chauffe;				// Puissance de chauffe calculée en sortie du régulateur 0-100%
};
//déclaration de la structure
stData stDonnees ;


typedef enum ETAT ETAT;
enum ETAT{
	ETAT_ERREUR, 	// 0
    ETAT_OFF, 		// 1
	ETAT_ON, 		// 2
	ETAT_AUTO		// 3
};

typedef enum MODE MODE;
enum MODE{
    MODE_ERREUR, 	// 0
	MODE_ATTENTE,	// 1
	MODE_ANTICIPE,	// 2
	MODE_CHAUFFE	// 3 
};

typedef enum TB_chauffage_status TB_chauffage_status;
enum TB_chauffage_status{
    TB_chauffage_status_ID, 
	TB_chauffage_status_ETAT,
	TB_chauffage_status_ETAT_PREC,
	TB_chauffage_status_MODE,
	TB_chauffage_status_MODE_PREC,
	TB_chauffage_status_CONSIGNE_UTILISATEUR,
	TB_chauffage_status_heure_debut_consigne,
	TB_chauffage_status_CONSIGNE_ANTICIPE
};

typedef enum ROW_chauffage_PID ROW_chauffage_PID;
enum ROW_chauffage_PID{
	ROW_id,
	ROW_D_prec,
	ROW_Consigne_prec,
	ROW_ecart_prec
};

typedef struct stPID stPID;
struct stPID
{
	float D_prec;
	float Consigne_prec;
	float ecart_prec;
};
//déclaration de la structure et initialisation
stPID stPID_poele = {0.0 , 0.0 , 0.0};
