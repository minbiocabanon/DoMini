//--------------------------------------------------
//! \file		sketche_poele.ino
//! \brief		Sketch for remote control of the Pellets Stove
//! \brief		Message received have this format :  $POL,x,yyy     x = 1 ou 0  , yyy = 0 à 100%
//! \brief		SYNCHRO : au démarrage, le poele doit être sur le menu standby (SB)
//! \date		2014-09
//! \author		minbiocabanon
//--------------------------------------------------

#include <JeeLib.h>
#include <MsTimer2.h>
#include <Timer.h>
#include <Ultrasonic.h>

#define version "JeeNode Poele"
char stNum[] = "POL";

#define POELE_OFF		0
#define POELE_ON		1
#define POELE_NOSTATE	3

//Declation des variables pour les GPIO
#define BTN_MENU		17		// Sortie pilotant bouton "Menu"  - PC3 pin 26 - DIGITAL 17
#define BTN_ENTER		4		//Sortie pilotant bouton "Enter"  - PD4 pin 6 - DIGITAL 4
#define BTN_PLUS		14		//Sortie pilotant bouton "Plus"  - PC0 pin 23 - DIGITAL 14
#define BTN_MOINS		7		//Sortie pilotant bouton "Moins"  - PD7 pin 13 - DIGITAL 7
#define BTN_SAC 		6		// Entrée captant le bouton "SAC"  - PD6 pin 12 - DIGITAL 6
// #define BTN_SYNC 		15		// Entrée captant le bouton "SAC"  -                    - DIGITAL 16

// ATTENTION, sur le poele cette patte est à 10V !  pour l'instant elle n'est pas reliée sur la carte
// Son utilisation n'est pas indispensable pour l'instant ....
#define BTN_ON_OFF 		5		// Sortie pilotant bouton "on_off"  - PD5 pin 11 - DIGITAL 5

#define NB_MENU		2
#define NB_SSMENU	9
#define PUISSANCE_MAXI	100
#define PUISSANCE_MINI	0

#define MODE_MANUEL 0

// Mode radio
// Values 2 and 3 can cause the millisecond time to lose a few interrupts. 
// Value 3 can only be used if the ATmega fuses have been set for fast startup, i.e. 258 CK - the default Arduino fuse settings are not suitable for full power down.
#define MODE_RADIO_NORMAL 0
#define MODE_RADIO_IDLE 1
#define MODE_RADIO_STANDBY 2
#define MODE_RADIO_PWR_DOWN 3
#define RADIO_SLEEP 0
#define RADIO_WAKEUP -1

#define ACK_TIME 5000
#define NB_ATTEMPTS_ACK 3

#define DELAY_APPUI_TOUCHE 200	//en milliseconde

#define NB_BLIP_LED	500			//nb de cycle à attendre entr 2 clignotements de la led status
#define NBCONSIGNE_RAZ 3		// nb de consigne à appliquer entre 2 recalage_zero

const int LED=15; 				//Port2 AIO  - DIGITAL 15 

enum MSG_POL{
	MSG_OFF, 		// 0
	MSG_ON,			// 1
	MSG_SYNC		// 2
	};

enum MENU{
	STANDBY, 		// 0
	ON,				// 1
	AUTOMATIQUE		// 2
	};

	
enum SOUSMENU{
	PRINCIPAL,		//0
	LUNDI,			//1
	MARDI,			//2
	MERCREDI,		//3
	JEUDI,			//4
	VENDREDI,		//5
	SAMEDI,			//6
	DIMANCHE,		//7
	PUISSANCE,		//8
	DATE_HEURE		//9
	};	


//prototype fonctions
int Trt_msg_POL(char *message);
int TrtReceptRadio(void);
int Trt_consigne(int consigne);
int standby_poele(void);
int sousmenu_goto(int index_sousmenu);
int menu_goto(int index_menu);
int set_puissance(int puissance);
int appui_bouton(int bouton);
void TimerMinute(void);
void NiveauGranules(void);
int scan_bouton(void);
int Trt_bouton(int bouton);
void tache_gestion_IHM(void);
void tache_mesure_niveau_granule(void);
void tache_gestion_radio(void);
void tache_gestion_puissance(void);
void it_bouton(void);
static byte waitForAck(void);
int send_data_radio(void);
void clignote_led(void);
void recalage_zero(void);

//Objets
//MilliTimer sendTimer;

//flags
boolean bMessageRecu = false;
boolean bflag_ManipManuelle = false;
boolean bflag_trt_btn = false;
boolean bflag_sac_pellet = false;
boolean bflag_mode_manuel = false;
boolean bflag_consigne_en_attente = false;
boolean bflag_recalage_0 = false;
boolean bflag_mesure_niveau = false;
boolean bflag_envoie_niveau = false;

// etat du poele et consigne de chauffe
int etat_poele; 		// 0 (OFF) ou 1 (ON)
int consigne_poele;		// 0 à 100 (%)
int pos_sousmenu = 0; 	// position dans le sousmenu
int pos_menu = 0; 		// position dans le menu
int pos_puissance = 0;	//  index sur la puissance du poele

int btn_it = 0;			// variable qui contient le N° de la patte ayant provoque l'IT
int nb_appuis = 0;		// variable pour DEBUG
int nblipled = 0;
int nNbConsigneCpt = 0; // variable pour compter le nombre de consignes appliquée afin de faire un recalage à 0 au bout de NBCONSIGNE_RAZ

int nb_sac_pellets = 0;	// variable pour le nombre de sacs
int niveau_granule = 0;	// variable pour le niveau du reservoir a granules

//chaine pour l'émission RF12
char buffer_recept_rf12[66]="";
char payload[16] = "$POL,XXX,N**";

Timer t;
Ultrasonic ultrasonic(16);	// Entrée captant l'info ultrason  AIO de P3 - DIGITAL 16

//----------------------------------------------------------------------
//!\brief           fait clignoter la led une fois
//!\return        -
//----------------------------------------------------------------------
void clignote_led(void){
	int i = 0;

	for(i=0; i<5; i++){
		//allume la led
		digitalWrite(LED, HIGH);
		delay(50);
		//eteint la led
		digitalWrite(LED, LOW);
		delay(100);
	}
}

//----------------------------------------------------------------------
//!\brief           positionne la puissance du poele à 0%
//!\brief           La puissance du poele se met en butee à 0% meme si on appui trop sur 'puissance moins', 
//!\brief 		donc on se considère dans le pire cas (100%) : il suffit d'appuyer 20 fois sur 'moins' pour etre à 0
//!\brief 		!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ATTENTION : IL FAUT ETRE DANS LE MENU "ON" ABSOLUMENT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!\return        -
//----------------------------------------------------------------------
void recalage_zero(void){

	// on appuie 20 fois sur la 'puissance moins'
	Serial.println("Recalage puissance 0% en cours... patientez ");
	int i=0;
	for (i=0; i<19; i++){
		// on appui sur le bouton puissance 'moins'
		appui_bouton(BTN_MOINS);
	}

	// on met la variable de puissance à 0
	pos_puissance = 0;
	
	//on indique qu'on a fait le recalage à 0	
	bflag_recalage_0 = true;
	
	//DEBUG
	Serial.println("\nPuissance recalee a 0% ");
}

//----------------------------------------------------------------------
//!\brief           Gestion de la réception des messages depuis la radio
//!\return        1 sir message reçu = VRL sinon 0
//----------------------------------------------------------------------
int TrtReceptRadio(void){

	 // si on a reçu un message valide
	if (rf12_recvDone() && rf12_crc == 0){
		//on copie le buffer reçu dans un buffer de travail
		memcpy(buffer_recept_rf12, (void*) rf12_data, rf12_len);
		// terminateur de chaine
		// à tester ???
		buffer_recept_rf12[rf12_len]=0;
						
		// DEBUG
		// Serial.println("\n Message recu");
		// Serial.print(buffer_recept_rf12);
		// Serial.println("<");
		//on verifie que l'entet du message est la bonne, en théorie c'est toujours le cas si on utilise les ID radio
		if( buffer_recept_rf12[1] == 'P' && buffer_recept_rf12[2] == 'O' && buffer_recept_rf12[3] == 'L'){
			//on affiche le message reçu
			//Serial.println(buffer_recept_rf12);
			//si l'émetteur du message veut un ACK, on lui envoie!
			if(RF12_WANTS_ACK){
				rf12_sendStart(RF12_ACK_REPLY, 0, 0);
				Serial.println("\n ACK sent");
			}
			
			// on retourne 1
			return(1);
		}
		else{
			//Serial.println("Msg recu non reconnu");
			// on retourne 0
			return(0);
		}
	}
	return(0);
}

//----------------------------------------------------------------------
//!\brief         Vérifie si le message reçu est un $POL
//!\param[in]     buffer radio reçu
//!\return        retourne 1 si le message est $POL, 0,XXX  sinon
//---------------------------------------------------------------------- 
int Trt_msg_POL(char *message){

 	//Déclaration des variables
	char *entete, *i;
	int type_msg_pol = 0;
	
	Serial.print("\nOn analyse le message recu");
	
	//on dépouille le message reçu
	entete = strtok_r(message, ",", &i);
	type_msg_pol = atoi(strtok_r(NULL, ",", &i)); 
	
	switch(type_msg_pol){
	// Cas ou l'on reçoit un message ON ou OFF
	case MSG_OFF:
	case MSG_ON:
		etat_poele = type_msg_pol;
		consigne_poele = atoi(strtok_r(NULL, ",", &i));
		//on affiche les donnees extraites
		Serial.print("\nEtat poele  : ");
		Serial.println(etat_poele);
		Serial.print("consigne poele  : ");
		Serial.println(consigne_poele);
		break;
	// c'est un message synchro 
	case MSG_SYNC:
		// on met le poele dans l'état indéfini
		etat_poele = POELE_NOSTATE;
		// on extrait la position du menu
		pos_menu = atoi(strtok_r(NULL, ",", &i));
		// on cale le sous-menu sur principal, position obligée sur on est sur ON/OFF/SB
		pos_sousmenu = PRINCIPAL;
		
		//on affiche les donnees extraites
		Serial.print("\nMessage de synchro. \n Position menu  : ");
		Serial.println(pos_menu);
		
		// le menu sur l'afficheur se mettra dans le bon état à la prochaine réception de consigne radio
		
		//on considère qu'on repart dans une configuration clean, donc on met le flag de recalage à 0% à false
		bflag_recalage_0 = false;
		
		break;
	}	
	return (1);
}

//----------------------------------------------------------------------
//!\brief           Traite la consigne de chauffe
//!\param[in]     consigne en %
//!\return        retourne 1 si OK
//---------------------------------------------------------------------- 
int Trt_consigne(int consigne){
		
	Serial.print("\nOn se repositionne au menu principal ");
	//on se place dans le menu PRINCIPAL
	sousmenu_goto(PRINCIPAL);
	Serial.print("\nPoele sur ON");
	//on se place dans le sous menu PRINCIPAL + ON 
	menu_goto(ON);
	
	Serial.print("\nOn applique la consigne au poele : ");
	Serial.println(consigne);
	
	//on applique la consigne
	set_puissance(consigne);
	
	return(1);
 }
 
//----------------------------------------------------------------------
//!\brief           Permet de se positionner dans le sous menu voulu
//!\param[in]     index du sousmenu dans lequel on veut aller
//!\return        retourne 1 si OK
//---------------------------------------------------------------------- 
int sousmenu_goto(int index_sousmenu){

	//tant que la position du sous menu est différent de la position souhaitée
	while(pos_sousmenu != index_sousmenu){
		//on incrément la position dans le menu
		pos_sousmenu ++;
		// on appui sur le bouton MENU
		appui_bouton(BTN_MENU);

		// on gère le débordement, les menus rebouclent au début
		if(pos_sousmenu > NB_SSMENU)
			pos_sousmenu = 0;	
	}
	//on retourne 1
	return(1);
}

//----------------------------------------------------------------------
//!\brief           Permet de se positionner dans le menu voulu
//!\param[in]     index du menu dans lequel on veut aller
//!\return        retourne 1 si OK
//---------------------------------------------------------------------- 
int menu_goto(int index_menu){

	//tant que la position du menu est différent de la position souhaitée
	while(pos_menu != index_menu){
		//on incrément la position dans le menu
		pos_menu ++;
		// on appui sur le bouton ENTER
		appui_bouton(BTN_ENTER);

		// on gère le débordement, les menus rebouclent au début
		if(pos_menu > NB_MENU)
			pos_menu = 0;	
	}
	//on retourne 1
	return(1);
}


//----------------------------------------------------------------------
//!\brief		Change la puissance du poele
//!\brief 		veiller à être dans le bon menu avant d'appeler cette fonction !!! sinon gare !
//!\param[in]    	puissance en %
//!\return		retourne 1 si OK
//---------------------------------------------------------------------- 
int set_puissance(int puissance){

	// Pour éviter la désynchronisation, toutes les N consignes, on fait un recalage à zero
	nNbConsigneCpt++;
	if( nNbConsigneCpt >= NBCONSIGNE_RAZ){
		bflag_recalage_0 = false;
		nNbConsigneCpt = 0;
	}

	// si le recalage n'a pas été fait (on se positionne à 0% au redémarrage pour se caler en puissance)
	if(bflag_recalage_0 == false)
		recalage_zero();

	//on regarde si la consigne est inférieure à la puissance en cours
	if(pos_puissance <= puissance && puissance < 100){
		//tant que l'index puissance  est inférieure  de la puissance souhaitée
		while(pos_puissance < puissance){
			// on incrémente la position dans le menu
			pos_puissance += 5;
			// on appui sur le bouton +
			appui_bouton(BTN_PLUS);
			// DEBUG
			Serial.print("puissance : ");
			Serial.println(pos_puissance);

			// on gère le débordement, les menus rebouclent au début
			if(pos_puissance > 100)
				pos_puissance = 100;	
		}
	}
	else{
		//tant que l'index puissance  est supérieure à la puissance souhaitée
		while(pos_puissance > puissance && puissance > 0){
			//on décrémente la position dans le menu
			pos_puissance -= 5;
			// on appui sur le bouton -
			appui_bouton(BTN_MOINS);
			//DEBUG
			Serial.print("puissance : ");
			Serial.println(pos_puissance);

			// on gère le débordement, les menus rebouclent au début
			if(pos_puissance <= 0)
				pos_puissance = 0;	
		}
	}
	//on retourne 1
	return(1);
}

//----------------------------------------------------------------------
//!\brief           Appui sur le bouton passé en argument
//!\param[in]     Bouton à appuyer
//!\return        retourne 1 si OK
//---------------------------------------------------------------------- 
int appui_bouton(int bouton){

	// on inhibe l'interruption pour ne pas se prendre une iT qd on va piloter une sortie
	//noInterrupts();
	//cli();
	detachInterrupt(1);

	//on passe la pin concernée en sortie
	pinMode(bouton, OUTPUT);
	
	//on active la sortie
	digitalWrite(bouton, 0);
	//on attend 500ms
	delay(DELAY_APPUI_TOUCHE);
	// on relache le bouton 
	digitalWrite(bouton, 1);
	//on attend 500ms
	delay(DELAY_APPUI_TOUCHE);

	//on passe la pin concernée en entrée
	pinMode(bouton, INPUT);
	
	// on autorise à nouveau les IT
	// interrupts();
	// sei();
	// on efface le flag à la main pour effacer les IT qui se seraient produites entre temps
	EIFR=(1<<INTF1); 
	attachInterrupt(1, it_bouton, FALLING);
	
	//on retourne 1
	return(1);
}

//----------------------------------------------------------------------
//!\brief           Passe le poele en mode standay
//!\return        retourne 1 si le message est $POL, 0 sinon
//---------------------------------------------------------------------- 
int standby_poele(void){

	Serial.print("\nOn attend la fin du timer ");

	// tant qu'on a pas attendu 1 minute aprés le dernier appui de l'utilisateur
	while(bflag_ManipManuelle == true){
		// on attend...
		delay(1);
	}
	
	Serial.print("\nOn se repositionne au menu principal ");
	//on se place dans le menu PRINCIPAL
	sousmenu_goto(PRINCIPAL);
	
	Serial.print("\nPoele en veille");
	//on se place dans le sous menu PRINCIPAL + ON 
	menu_goto(STANDBY);
	
	return(1);
 }
 
//----------------------------------------------------------------------
//!\brief           Scrute chaque entrée pour savoir quel est le bouton enfonce
//!\return        retourne le N° (DIGITAL) du bouton appuye
//---------------------------------------------------------------------- 
int scan_bouton(void){

	int btn = 0;
	
	//--------- On cherche à savoir quel entrée à provoquer l'IT
	if(digitalRead(BTN_MENU) == 0 ){
		btn = BTN_MENU;
	}
	else if (digitalRead(BTN_ENTER)  == 0){
		btn = BTN_ENTER;
	}
	else if (digitalRead(BTN_PLUS)  == 0){
		btn = BTN_PLUS;
	}
	else if (digitalRead(BTN_MOINS)  == 0){
		btn = BTN_MOINS;
	}
	else if (digitalRead(BTN_SAC)  == 0){
		btn = BTN_SAC;
	}
	// else if (digitalRead(BTN_SYNC)  == 0){
		// btn = BTN_SYNC;
	// }	
	// else if (digitalRead(BTN_ON_OFF)  == 0){
		// btn = BTN_ON_OFF;
	// }
	return(btn);	
}

//----------------------------------------------------------------------
//!\brief           Position les variables internes pour savoir ou nous en sommes dans le menu 
//!\param[in]     bouton a appuyer
//!\return        ?
//---------------------------------------------------------------------- 
int Trt_bouton(int bouton){

	switch(bouton){
	
		case BTN_MENU: 
			// on passe au sous menu suivant
			pos_sousmenu++;
			//si on déborde
			if(pos_sousmenu > NB_SSMENU)
				//on se remet au début du sous menu
				pos_sousmenu = 0;
			break;
		case BTN_ENTER: 
			//si on est dans le menu PRINCIPAL
			if(pos_sousmenu == PRINCIPAL){
				// on passe au menu suivant
				pos_menu++;
				//si on déborde
				if(pos_menu > NB_MENU)
					//on se remet au début du sous menu
					pos_menu = 0;
			}
			break;
		case BTN_PLUS: 
			//si on est dans le menu ON + sous menu PRINCIPAL
			if(pos_menu == ON && pos_sousmenu == PRINCIPAL){
				// on augmente la puissance par pas de 5%
				pos_puissance += 5;
				//si on déborde
				if(pos_puissance > PUISSANCE_MAXI)
					//on limite au max de puissance
					pos_puissance = PUISSANCE_MAXI;
					
				//on positionne un flag indiquaut que l'utilisateur a decidé de passer en régulation manuellement
				bflag_mode_manuel = true;
			}			
			break;
		case BTN_MOINS: 
			//si on est dans le menu ON + sous menu PRINCIPAL
			if(pos_menu == ON && pos_sousmenu == PRINCIPAL){
				// on diminue la puissance par pas de 5%
				pos_puissance -= 5;
				//si on déborde
				if(pos_puissance < PUISSANCE_MINI)
					//on limite au max de puissance
					pos_puissance = PUISSANCE_MINI;
				
				//on positionne un flag indiquaut que l'utilisateur a decidé de passer en régulation manuellement
				bflag_mode_manuel = true;
			}			
			break;
		case BTN_ON_OFF: 
			// là y'a pas grand chose a faire puisque l'utilisateur vient d'eteindre le poele (le con!)
			Serial.println("OFF !!!!!!!!!!!! ");
			// RAPPEL : cette patte est à 10V,  elle n'est pas reliées au Jeenode
			break;			
		case BTN_SAC: 
			// L'utilisateur indique qu'il vient de mettre un sac de granulés dans le réservoir du poele
			Serial.println("+1 sac de pellets");
			nb_sac_pellets++;			
			break;	
		// case BTN_SYNC: 
			// // L'utilisateur resynchronise le menu
			// Serial.println("Synchro menu ");
			// //on se remet au début du sous menu
			// pos_sousmenu = 0;
			// //on se remet au début du sous menu
			// pos_menu = 0;
			// break;				
	}
	
	//DEBUG
	Serial.print("Position MENU: ");
	Serial.println(pos_menu);
	Serial.print("Position SS MENU: ");
	Serial.println(pos_sousmenu);
	Serial.print("Puissance: ");
	Serial.println(pos_puissance);
	
}

//----------------------------------------------------------------------
//!\brief		Gestion de l'attente de la réception du ACK
//!\param	 	Buffer à envoyer
//!\return		1 si ACK recu , 0 sinon
//---------------------------------------------------------------------- 
static byte waitForAck() {
	MilliTimer ackTimer;
	while (!ackTimer.poll(ACK_TIME)){
		if (rf12_recvDone() && rf12_crc == 0 && ((rf12_hdr & RF12_HDR_ACK) == 0) && ((rf12_hdr & RF12_HDR_CTL) == 128) ){
			Serial.println("ACK Received");
			Serial.print("Node ID:");Serial.println(rf12_hdr & RF12_HDR_MASK); 
			// Serial.println("received something");
			// Serial.println(rf12_hdr);
			// Serial.print("RF12_HDR_DST=");Serial.println(rf12_hdr & RF12_HDR_DST);
			// Serial.print("RF12_HDR_CTL=");Serial.println(rf12_hdr & RF12_HDR_CTL);
			// Serial.print("RF12_HDR_ACK=");Serial.println(rf12_hdr & RF12_HDR_ACK);
			// Serial.print("rf12_len=");Serial.println(rf12_len);
			return 1;
		}
	}
	return 0;
}

//----------------------------------------------------------------------
//!\brief		Transmet les données du buffer radio
//!\param	 	Buffer à envoyer
//!\return		?
//---------------------------------------------------------------------- 
//int send_data_radio(char *message){
int send_data_radio(void){
	int nAttempt = 1;
	bool flag_ACK_received = false;		

	//DEBUG
	Serial.println("\nBuffer radio a emettre: ");
	for(byte i=0; i <= strlen(payload); i++)
		Serial.print(payload[i]);
	Serial.println();
		
	while(nAttempt < NB_ATTEMPTS_ACK && !flag_ACK_received ){
		
		//tant que porteuse pas libre
		while (!rf12_canSend())
			//on interroge le module
			rf12_recvDone();
		// porteuse libre, on envoie le message
		Serial.println("Envoi message en cours...");
		rf12_sendStart(RF12_HDR_ACK, payload, sizeof payload);
		rf12_sendWait(1);
		Serial.println("Message envoye OK");

		Serial.print("Nb tentatives : ");Serial.println(nAttempt);
		
		if (waitForAck()){
			Serial.println("ACK recu\n");
			flag_ACK_received = true;
		}else {Serial.println("ACK non recu\n");}		
		nAttempt++;
	}
	// on attend un peu ... plus (au cas ou on aurait plusieurs messages à transmettre)
	delay(100);
}

//----------------------------------------------------------------------
//!\brief           Tache de fond qui gère tout ce qui se passe sur les boutons
//---------------------------------------------------------------------- 
void tache_gestion_IHM(void){

	//on a appuye sur un des boutons
	if(bflag_trt_btn == true){
	
		Serial.println("tache_gestion_IHM");
	
		//on lance le timer de 15 sec
		MsTimer2::start();

		//pour anti rebond, on inhibe les IT
		detachInterrupt(1);
		
		// on scan tous les boutons pour savoir lequel est enfoncé
		btn_it = scan_bouton();
				
		//on efface le flag
		bflag_trt_btn = false;
		
		//anti rebond sous forme de clignotage de led
		//allume la led
		digitalWrite(LED, LOW);
		delay(25);
		//eteint la led
		digitalWrite(LED, HIGH);
		delay(25);
		
		// on efface le flag à la main pour effacer les IT qui se seraient produites entre temps
		EIFR=(1<<INTF1); 
		//on re autorise les IT
		attachInterrupt(1, it_bouton, FALLING);	
		
		//maintenant on fait les taches plus longues à réaliser... (pour ne pas perdre d'IT)
		
		Serial.print("Bouton appuye : ");
		Serial.println(btn_it);	
		// Serial.print("Nb appuis : ");
		// Serial.println(nb_appuis++);	
		
		//on traite l'action a realiser selon le bouton appuye
		Trt_bouton(btn_it);
	}	
}

//----------------------------------------------------------------------
//!\brief           Tache de fond qui gère la radio
//---------------------------------------------------------------------- 
void tache_gestion_radio(void){
	
	// char payload[]="$POL,XXX,N**";
	//char payload[16];

	//si le message radio est correcte -> message radio ($POL)   ET que l'utilisateur n'est pas en train de trifouiller les boutons . on ne traite pas la consigne reçu par radio si l'utilisateur s'apprete à changer la consigne (passage en mode manuel)
	if (TrtReceptRadio() == 1  && bflag_ManipManuelle == false){
		//on execute la consigne du message recu
		Trt_msg_POL(buffer_recept_rf12);
		// on indique qu'une consigne est à traiter
		bflag_consigne_en_attente = true;
		// on fait clignoter la led pour le debug
		clignote_led();
	}
	
	// si l'utilisateur a mis un ou des sacs dans le reservoir, il faut envoyer un message du sheevaplug
	if(bflag_sac_pellet == true){
		//definition du message : PPP = puissance en %       N=nb de sac mis dans le réservoir;
		//char payload[]="$POL,SAC,N**";
		
		//on compile les donnees dans le buffer a transmettre
		sprintf(payload,"!%s,SAC,%1d\r\n", stNum, nb_sac_pellets);
		//DEBUG
		Serial.println(payload);
			 
		//on envoi le message
		send_data_radio();
		
		// on remet le nombre de sac à 0
		nb_sac_pellets = 0;
		// on reset le flag 
		bflag_sac_pellet = false;
		
		// on fait clignoter la led pour le debug
		clignote_led();
	}
	
	// si on doit envoyer la valeur du niveau du reservoir a granule
	if(bflag_envoie_niveau == true){
		//definition du message : NNN = mesure en centimetre du niveau. 000cm = reservoir plein.
		//char payload[]="$POL,NVG,000**";
		
		//on compile les donnees dans le buffer a transmettre
		sprintf(payload,"$%s,NVG,%3d\r\n", stNum, niveau_granule);
		//DEBUG
		Serial.println(payload);
			 
		//on envoi le message
		send_data_radio();
		
		// on reset le flag 
		bflag_envoie_niveau = false;
	}
	
	//si l'utilisateur a modifier la puissance manuellement, il faut avertir la centrale domotique pour arrêter la régulation automatique
	if(bflag_mode_manuel == true && bflag_ManipManuelle == false){
		//definition du message : PPP = puissance en %       N=nb de sac mis dans le réservoir;
		//char payload[]="$POL,MOD,N**";
		
		//on compile les donnees dans le buffer a transmettre
		sprintf(payload,"$POL,MOD,%1d\r\n", MODE_MANUEL);

		//DEBUG
		Serial.println(payload);
		//on envoi le message
		//send_data_radio(payload);
		send_data_radio();
				
		// on reset le flag
		bflag_mode_manuel = false;
	}
}

//----------------------------------------------------------------------
//!\brief           Tache de fond qui mesure le niveau du reserver a granules
//---------------------------------------------------------------------- 
void tache_mesure_niveau_granule(){
	// si le timer a claque
	if( bflag_mesure_niveau == true){
		//on realise la mesure
		unsigned int x = -1;
		unsigned int i = 0;
		//on fait une premiere mesure
		ultrasonic.MeasureInCentimeters();
		x = ultrasonic.RangeInCentimeters;
		// on refait la mesure tant qu'elle est incoherente ou si on fait plus de 10 mesure
		while( !(x > 0 && x <= 150) && i < 10){
			ultrasonic.MeasureInCentimeters();
			x = ultrasonic.RangeInCentimeters;
			i++;
			Serial.print("(while)Niveau granule : mesure ultrason (cm) = ");
			Serial.println(x);
			delay(100);
		}
		if(i < 10){
			// on prepare le flag pour envoyer la mesure par radio
			bflag_envoie_niveau = true;
			niveau_granule = x;
			// on affiche du debug
			Serial.print("Niveau granule : mesure ultrason (cm) = ");
			Serial.println(niveau_granule);
		}
		else{
			// on affiche du debug
			Serial.print("Niveau granule : Erreur sur la mesure ");
			//sinon on force a false
			bflag_envoie_niveau = false;
		}
		// on reset le flag 
		bflag_mesure_niveau = false;	
	}
}

//----------------------------------------------------------------------
//!\brief           Tache de fond qui gère la puissance appliquée au poele
//---------------------------------------------------------------------- 
void tache_gestion_puissance(void){

	// si une consigne est en attente de traitement ET  que le délai d'attente est bien expiré  : on peut appliquer la consigne 
	if(bflag_consigne_en_attente == true && bflag_ManipManuelle == false){
		
		//si le poele est allume + consigne
		if (etat_poele == POELE_ON){	
			//on execute la consigne du message recu
			Trt_consigne(consigne_poele);
			//on met le poele dans l'état non defini pour la prochaine boucle
			etat_poele = POELE_NOSTATE;
		}
		else if (etat_poele == POELE_OFF){
			// on éteind le poele
			standby_poele();
			//on met le poele dans l'état non defini
			etat_poele = POELE_NOSTATE;
			//on remet le flag de recalage à 0 pour qu'au prochaine allumage du poele, on repasse à 0% pour se recaler
			bflag_recalage_0 = false;
		}
	
		// on indique qu'on a traité la consigne
		bflag_consigne_en_attente = false;
	}
}

//----------------------------------------------------------------------
//!\brief           fait clignoter une led en fond de tache pour donner un signe de vie
//!\return        -
//----------------------------------------------------------------------
void status(void){
	
	nblipled++;
	if(nblipled > NB_BLIP_LED){
		//allume la led
		digitalWrite(LED, HIGH);
		delay(50);
		//eteint la led
		digitalWrite(LED, LOW);
		nblipled=0;		
	}
}

//----------------------------------------------------------------------
//!\brief           Timer d'une minute
//----------------------------------------------------------------------
void TimerMinute(void) {
	// on indique que la minute est passée depuis le dernier appui sur un bouton -> reset du flag
	bflag_ManipManuelle = false;
	Serial.println("\n MINUTE ECOULEE! ");
	
	// cas particulier : on regarde si l'utilisateur a mis des sacs de granulés dans le réservoir
	if(nb_sac_pellets > 0){
		//on lève le flag pour envoyer un message radio vers le sheevaplug
		bflag_sac_pellet = true;
	}
		
	//on arrête le timer
	MsTimer2::stop();
}

//----------------------------------------------------------------------
//!\brief           Interruption lors d'un appui sur un bouton
//----------------------------------------------------------------------
 void it_bouton(void) {
	// on indique qu'un bouton a été pressé pour interdire la manipulation des boutons (risque de conflit avec l'utilisateur si il est en train de tapoter sur les touches)
	bflag_ManipManuelle = true;
	// on leve le flag pour lancer le traitement de l'appui sur le bouton
	bflag_trt_btn = true;
}

//----------------------------------------------------------------------
//!\brief           Interruption lorsque le timer 10 minutes pour la mesure de niveau expire
//----------------------------------------------------------------------
void NiveauGranules(){
	// On n'envoies le niveau que si le poele est ON, en principe si le poele est OFF, le niveau ne bouge pas !
	if(etat_poele == POELE_ON)
		bflag_mesure_niveau = true;
}
 
//----------------------------------------------------------------------
//!\brief           INITIALISATION
//----------------------------------------------------------------------
void setup (void) {
	
	// init port COM du debug
	Serial.begin(57600);
	// affichage de la version
	Serial.println(version);
	Serial.println();
	Serial.println("Num. de Jeenode : ");
	Serial.println(stNum);
	  
	//config des GPIO
	pinMode(BTN_MENU, INPUT);
	pinMode(BTN_ENTER, INPUT);
	pinMode(BTN_PLUS, INPUT);
	pinMode(BTN_MOINS, INPUT);
	pinMode(BTN_ON_OFF, INPUT);
	pinMode(BTN_SAC, INPUT);
	// pinMode(BTN_SYNC, INPUT);
	

	// ------- Broches en sortie -------  
	pinMode(LED, OUTPUT); //met la broche en sortie 
	digitalWrite(LED, LOW);
	
	clignote_led();
		
	//on gère une patte comme interruption hardware -> INT1 = PD3 , Pin IRQ
	attachInterrupt(1, it_bouton, FALLING);
	
	//on configure le timer, mais on ne le lance pas !
	MsTimer2::set(15000, TimerMinute); // 15s period
	// on arme le timer de 10 minutes pour la mesure de niveau du réservoir à granulés
	t.every(600000, NiveauGranules);	// 10 minutes (attention, ne pas ecrire 1000 * 60 * 10 -> ne fonctionne pas !
	
	//initialisation du module radio RF12
	rf12_initialize(1, RF12_868MHZ, 33);
	// Reconfig du baudrate : 1200 bauds
	rf12_control(0xC6A3);	
	Serial.print("\nInit Radio : \n 1200 bauds\n");
		
	Serial.println("\nFin SETUP");
	
}

//----------------------------------------------------------------------
//!\brief           BOUCLE PRINCIPAL
//----------------------------------------------------------------------
void loop(){

	tache_gestion_IHM();
	
	tache_gestion_puissance();
	
	tache_gestion_radio();
	
	tache_mesure_niveau_granule();
	
	status();
	
	delay(10);
	
	t.update();
}