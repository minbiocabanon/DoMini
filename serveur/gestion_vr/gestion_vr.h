//----------------------------------------------------------------------
//!\file		gestion_vr.h
//!\date
//!\author		Minbiocabanon
//----------------------------------------------------------------------

// prototypes des fonctions utilisées
void sigfun(int sig);
int init_mysql(void);
int recup_info_jour(void);
int etat_vr(int etat_bureau, int etat_salon, int etat_chm, int etat_chjf);
float Calcul_fluxsolaire(void);
int envoie_msg_etat_vr(int vr_bureau, int vr_salon, int vr_chm, int vr_chjf);
int recup_temperature(char *saison);
int etat_vr_mode_auto(void);
int ecrire_etat_vr_table(int vr_bureau, int vr_salon, int vr_chm, int vr_chjf);

typedef struct stMydata stMydata;
struct stMydata{
	unsigned int seconds ; 				//date/heure au format UNIXTIME
	float consigne_temperat;
	float temp_int;
	unsigned int flux_solaire_trig;
	unsigned int flag_flux ;			// flag indiquant si la condition sur le flux solaire est vrai ou fausse	
	unsigned int flag_temperature ;		// flag indiquant si la condition sur la température de consigne est remplie ou non
	unsigned int h_lever ;				// heure de lever du soleil en unixtime
	unsigned int h_coucher ;			// heure de coucher du soleil en unixtime
	unsigned int flag_jour;				// flag indiquant si il fait jour (flag =1) ou nuit(flag =0)
};
//déclaration de la structure
stMydata stDonnees ;

typedef struct stData stData;
struct stData{
	unsigned int mode; 				//mode de gestion : Automatique ou Manuel
	unsigned int etat_vr_bureau; 	//Etat du volet dans le bureau
	unsigned int etat_vr_salon; 	//Etat du volet dans le bureau
	unsigned int etat_vr_chm;  		//Etat du volet dans le bureau
	unsigned int etat_vr_chjf;  	//Etat du volet dans le bureau
};
//déclaration de la structure
stData stEtatVR ;

typedef enum VOLET VOLET;
enum VOLET{
	FERME, 		// 0
	OUVERT,		// 1
	MIOMBRE,	// 2
	IMMOBILE	// 3
	};
	
typedef enum MODE_VR MODE_VR;
enum MODE_VR{
	AUTOMATIQUE, 	// 0
	TOUS_FERME,		// 1
	TOUS_OUVERT,	// 2
	MANUEL			// 3
	};