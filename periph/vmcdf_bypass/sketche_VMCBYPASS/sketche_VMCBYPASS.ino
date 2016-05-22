//--------------------------------------------------
//! \file     sketche_VMCBYPASS.ino
//! \brief    Sketch pour le JeeNode : gestion de la VMC et du bypass
//! \brief 	Les messages reçus ont ce format:  $BPC,xxx,y     xxx = 0 à 100%  , y = 0 ou 1
//! \brief 	xxx = 0% -> bypass en position PUITS CANADIEN
//! \brief 	xxx = 100% -> bypass en position AIR EXTERIEUR
//! \date     2014-6
//! \author   minbiocabanon
//--------------------------------------------------

#include <JeeLib.h>
#include <PortsSHT11.h>
#include <MsTimer2.h>
#include <math.h>

#define version "JeeNode VMCDF+Bypass"
char stNum[] = "VMC";

#define NBMINUTEVEILLE	300 * 10	// x10 car basé sur Timer100ms
#define TIME_0_TO_100	2 * 10		//Nb de seconde (x10 car basé sur Timer100ms)  que mets le registre à parcourir la totalité de sa course
#define TIMEOUTMOTEUR_SEC	5 * 10	// Nb de seconde (x10 car basé sur Timer100ms) pour le TimeOut à partir duquel on stoppe automatiquement les moteurs
#define NB_BLIP_LED	500			//nb de cycle à attendre entr 2 clignotements de la led status

// Mode radio
// Values 2 and 3 can cause the millisecond time to lose a few interrupts. 
// Value 3 can only be used if the ATmega fuses have been set for fast startup, i.e. 258 CK - the default Arduino fuse settings are not suitable for full power down.
#define MODE_RADIO_NORMAL 0
#define MODE_RADIO_IDLE 1
#define MODE_RADIO_STANDBY 2
#define MODE_RADIO_PWR_DOWN 3
#define RADIO_SLEEP 0
#define RADIO_WAKEUP -1

//Constante pour la consersion tension -> °C
#define RTALON	15900.00
#define UALIM	3.3
#define R25		22000.00
#define B		3740
#define Q		3.3/1024

//Declation des variables pour les GPIO
#define CMD_EXT		4				// Commande Bypass vers Ext sur PD5 - Pin11
#define CMD_PC		5				// Commande Bypass vers Ext sur PD4 - Pin6
#define CMD_VMCDF	7				// Commande Bypass vers Ext sur PD7 - Pin13
#define INFO_PC		17				// Commande Bypass vers Ext sur PC4 - Pin27	
#define INFO_EXT	26	//FAUX			// Commande Bypass vers Ext sur PC3 - Pin26


//prototype fonctions
void tache_gestion_radio(void);
void setup(void);
int Trt_msg_BPC(char *message);
void SetVMC(int consigne_VMC);
void Timer100ms(void);
void CaptureSensor(void);
void SendRadioData(void);
int TrtReceptRadio(void);
void SetBypass(int consigne_bypass);
void clignote_led(void);

//variable pour timer
unsigned long TicksMsgRad = 0;
unsigned long TicksMoteur = 0;
unsigned long TimeOutMoteur = 0;
boolean bFlagMoteurPosOK = false;
int nblipled;


//variable capteurs SHT11
float h, t, d;
//variables pour la lecture du CAN interne
int T_AIR_NEUF = 0;    // Selection de l'entree analog de mesure de la température air neuf
int T_AIR_VICIE = 1;    // Selection de l'entree analog de mesure de la température air vicié
int byTemp_air_vicie;
int byTemp_air_neuf;

int consigne_bypass, consigne_VMC;

//flag
boolean bTimeToSendRadioMsg = false;
boolean bflag_consigne_en_attente = false;

//chaine pour l'émission RF12
char buffer_recept_rf12[66]="";

//Declaration du capteur SHT11 sur le port3 du jeenode (I2C soft)
SHT11 hsensor (3);

int consigne_prec = 0;			//Position courante du Bypass

const int LED=14; 				// PC0 - pin23 - AOI Port 1


//----------------------------------------------------------------------
//!\brief           fait clignoter la led une fois
//!\return        -
//----------------------------------------------------------------------
void clignote_led(void){
	int i = 0;
	for(i=0; i<5; i++){
		//allume la led
		digitalWrite(LED, LOW);
		delay(50);
		//eteint la led
		digitalWrite(LED, HIGH);
		delay(100);
	}
}

//----------------------------------------------------------------------
//!\brief		Timer 100 milliseconde
//!\return		-
//---------------------------------------------------------------------- 
void Timer100ms(void){

	//si le timer moteur est lancé
	if ( TicksMoteur != 0){
		// on décrémente le compteur
		TicksMoteur--;
		//debug
		Serial.println(TicksMoteur);
		//Si le timer a expiré, on a du atteindre la position
		if( TicksMoteur == 0)
			bFlagMoteurPosOK = true;
	}
	
	// on incrémente le compteur de ticks
	TicksMsgRad++;
	//Si le timer a expiré, on set le flag pour envoyer un message radio
	if (TicksMsgRad >= NBMINUTEVEILLE){
		// on set le flag pour envoyer le message radio
		bTimeToSendRadioMsg = true;
		// on reset le compteur
		TicksMsgRad = 0;
	}
	
	//si le timer antiblocage moteur est lancé
	if ( TimeOutMoteur != 0){
		// on décrémente le compteur
		TimeOutMoteur--;
		//Si le timer a expiré, on a du atteindre la position
		if( TimeOutMoteur == 0){
			// on stoppe les moteurs
			digitalWrite(CMD_EXT, LOW);
			digitalWrite(CMD_PC, LOW);
		}
	}
}

//----------------------------------------------------------------------
//!\brief		Fait l'acquisition des capteurs
//!\return		-
//---------------------------------------------------------------------- 
void CaptureSensor(void){
	
	//variable pour les valeurs analogiques
	double dbTensionVolt;
	int byTensionHEX = 0;  // variable to store the value coming from the sensor
	
	// **************************************************
	// *	Mesure de la temperature air neuf
	// **************************************************
	// Lecture de la valeur du CAN interne de l'ATmega
	byTensionHEX = analogRead(T_AIR_NEUF);
	//DEBUG
	// Serial.print("\nVolt HEXA. : ");
	// Serial.print(byTensionHEX,HEX);
	// Serial.print("-> DEC. : ");
	// Serial.print(byTensionHEX,DEC);
	
	dbTensionVolt = byTensionHEX * Q ;
	double dbTemp_air_neuf = -273.15 + 1 / ( (log( (dbTensionVolt * RTALON / (UALIM - dbTensionVolt) ) / R25) )/ B + ( 1 / (273.15 + 25)));
	
	//DEBUG
	// Serial.print("\nVolt dbvolt. : ");
	// Serial.print(dbTensionVolt,4);
	//conversion du float en INT x100 , x100 pour supprimer la virgule et envoyer un entier
	byTemp_air_neuf = dbTemp_air_neuf * 100;	
	//DEBUG
	// Serial.print("\nVolt byvolt. : ");
	// Serial.print(byTensionVolt,DEC);

	// **************************************************
	// *	Mesure de la temperature air vicié
	// **************************************************
	// Lecture de la valeur du CAN interne de l'ATmega
	byTensionHEX = analogRead(T_AIR_VICIE);
	//DEBUG
	// Serial.print("\nVolt HEXA. : ");
	// Serial.print(byTensionHEX,HEX);
	// Serial.print("-> DEC. : ");
	// Serial.print(byTensionHEX,DEC);
	
	dbTensionVolt = byTensionHEX * Q ;
	double dbTemp_air_vicie = -273.15 + 1 / ( (log( (dbTensionVolt * RTALON / (UALIM - dbTensionVolt) ) / R25) )/ B + ( 1 / (273.15 + 25)));
	
	//DEBUG
	// Serial.print("\nVolt dbvolt. : ");
	// Serial.print(dbTensionVolt,4);
	//conversion du float en INT x100 , x100 pour supprimer la virgule et envoyer un entier
	byTemp_air_vicie = dbTemp_air_vicie * 100;	
	//DEBUG
	// Serial.print("\nVolt byvolt. : ");
	// Serial.print(byTensionVolt,DEC);

	
	// **************************************************
	// *	Lecture des données dans le capteur T° + H° 
	// **************************************************
	
	uint8_t error = hsensor.measure(SHT11::HUMI);        
	error |= hsensor.measure(SHT11::TEMP);
	// Serial.print(hsensor.meas[SHT11::TEMP]);
	//Serial.print(error, DEC);

	// omit following code to avoid linking in floating point code

	hsensor.calculate(h, t);
	// Serial.print("\nHumid. : ");
	// Serial.print(h);
	// Serial.print("%\nTemp. :");
	// Serial.print(t);
		
	// omit following code to avoid linking in the log() math code
	d = hsensor.dewpoint(h, t);
	// Serial.print("C\nPoint de rosee :");
	// Serial.print(d);
	// Serial.print("C\n\n");
}

//----------------------------------------------------------------------
//!\brief		Transmet les données du buffer radio
//!\param	 	Buffer à envoyer
//!\return		?
//---------------------------------------------------------------------- 
void SendRadioData(void){
	// **************************************************
	// *	Compilation des données dans un message
	// **************************************************
	
	// conversion du string en buffer pour la radio
	char payload[29]="";
	
	//conversion du float en INT x100
	int bytemp = t * 100;
	int byhumid = h * 100;
	int bydewpt = d * 100;
 	sprintf(payload,"$%s,%04d,%04d,%04d,%04d\r\n", stNum, byTemp_air_vicie, byTemp_air_neuf, bytemp, byhumid);
	

	// **************************************************
	// *	Emission des infos  sur la radio
	// **************************************************
	
	//DEBUG
	Serial.println("\nBuffer radio a emettre: ");
	for(byte i=0; i <= strlen(payload); i++)
		Serial.print(payload[i]);
	Serial.println();
	
	//A executer régulièrement 
	rf12_recvDone();
	
	//tant que porteuse pas libre
	while (!rf12_canSend())
		//on interroge le module
		rf12_recvDone();
	// porteuse libre, on envoie le message
	rf12_sendStart(0, payload, strlen(payload));
	Serial.print("\n\rMessage envoye OK\n\r");
	
	//attente de la fin d'émission radio
	rf12_sendWait(MODE_RADIO_IDLE);
}

//----------------------------------------------------------------------
//!\brief           Gestion de la réception des messages depuis la radio
//!\return        1 sir message reçu = VRL sinon 0
//----------------------------------------------------------------------
int TrtReceptRadio(void){

	int RetVal =0;
	// si on a reçu un message valide
	if (rf12_recvDone() && (rf12_crc == 0)){
		
		//on copie le buffer reçu dans un buffer de travail
		memcpy(buffer_recept_rf12, (void*) rf12_data, rf12_len);
		// terminateur de chaine
		// à tester ???
		buffer_recept_rf12[rf12_len]=0;

		// DEBUG
		Serial.println("\n Message recu");
		Serial.print(buffer_recept_rf12);
		 Serial.println("<");	 
		//on verifie que l'entet du message est la bonne, en théorie c'est toujours le cas si on utilise les ID radio
		if( buffer_recept_rf12[0] == '$' && buffer_recept_rf12[1] == 'B' && buffer_recept_rf12[2] == 'P' && buffer_recept_rf12[3] == 'C'){
			//on affiche le message reçu
			//Serial.println(buffer_recept_rf12);
			//si l'émetteur du message veut un ACK, on lui envoie!
			if(RF12_WANTS_ACK){
				rf12_sendStart(RF12_ACK_REPLY, 0, 0);
				Serial.println("\n ACK sent");
			}			
			// on retourne 1
			RetVal = 1;
		}
		else{
		 Serial.println("Msg recu non reconnu");
		 // on retourne 0
		}
	}
	return(RetVal);
}

//----------------------------------------------------------------------
//!\brief          Positionne la bypass
//!\param[in]    	consigne en %
//----------------------------------------------------------------------
void SetBypass(int consigne_bypass){
	//DEBUG
	Serial.print("on applique la consigne Bypass : ");
	Serial.println(consigne_bypass);
	
	int delta = consigne_prec - consigne_bypass;
	
	if(consigne_bypass > 95){
		digitalWrite(CMD_EXT, HIGH);
		digitalWrite(CMD_PC, LOW);
	}else if (consigne_bypass < 5){
		digitalWrite(CMD_EXT, LOW);
		digitalWrite(CMD_PC, HIGH); 		
	}else{
		if (delta >=0){
			digitalWrite(CMD_EXT, HIGH);
			digitalWrite(CMD_PC, LOW);
			
		}else{
			digitalWrite(CMD_EXT, LOW);
			digitalWrite(CMD_PC, HIGH); 
		}
		
		//On lance un timer de la durée nécessaire pour positionner le registre
		//on charge le compteur du timer moteur avec la valeur 
		TicksMoteur =  TIME_0_TO_100 * abs(delta) / 100;	 // (x10 car basé sur Timer100ms)
		
		Serial.print("Timer lance (s): ");
		Serial.println(TicksMoteur);
		//on memorise la consigne courante 
		consigne_prec = consigne_bypass;
	}
}

//----------------------------------------------------------------------
//!\brief          Allume ou éteind la VMC
//!\param[in]    	consigne ON/OFF
//----------------------------------------------------------------------
void SetVMC(int consigne_VMC){
	//DEBUG
	Serial.print("on applique la consigne VMC : ");
	Serial.println(consigne_VMC);
	digitalWrite(CMD_VMCDF, consigne_VMC);   // on applique la consigne
}

//----------------------------------------------------------------------
//!\brief          Traite le message des consignes VMC et bypass
//!\param[in]    	message reçu
//----------------------------------------------------------------------
int Trt_msg_BPC(char *message){

	//Déclaration des variables
	char *entete, *i;
	//on dépouille le message reçu
	entete = strtok_r(message, ",", &i);
	consigne_bypass = atoi(strtok_r(NULL, ",", &i)); 
	consigne_VMC = atoi(strtok_r(NULL, ",", &i));
	
	return(1);
}

//----------------------------------------------------------------------
//!\brief          Tache qui gère l'envoi des messages avec les données capteurs
//!\param[in]    	message reçu
//----------------------------------------------------------------------
void tache_gestion_capteurs(void){
	// si le timer de 5 minutes a expiré
	if( bTimeToSendRadioMsg == true){
		//DEBUG
		Serial.print("\nFlag IT TIMER !!!\n");
		// on capture les données des capteurs
		CaptureSensor();
		// on envoir les données vers la radio
		SendRadioData();
		// reset du flag timer
		bTimeToSendRadioMsg = false;
	}
}

//----------------------------------------------------------------------
//!\brief           Tache de fond qui gère la radio
//---------------------------------------------------------------------- 
void tache_gestion_radio(void){

	//si le message radio est correcte -> message radio ($POL)   ET que l'utilisateur n'est pas en train de trifouiller les boutons . on ne traite pas la consigne reçu par radio si l'utilisateur s'apprete à changer la consigne (passage en mode manuel)
	if (TrtReceptRadio() == 1){
		//on execute la consigne du message recu
		Trt_msg_BPC(buffer_recept_rf12);
		// on indique qu'une consigne est à traiter
		bflag_consigne_en_attente = true;		
		// on fait clignoter la led pour le debug
		clignote_led();
	}
}

//----------------------------------------------------------------------
//!\brief           Tache de fond qui gère les consignes
//---------------------------------------------------------------------- 
void tache_gestion_consignes(void){

	// si une consigne est en attente de traitement 
	if(bflag_consigne_en_attente == true ){
		//on execute la consigne pour la VMC
		SetVMC(consigne_VMC);
		//on execute la consigne pour le bypass
		SetBypass(consigne_bypass);
		//on arme un TimeOut au cas ou un des moteurs reste bloqué (si le timer expire on arrête les deux moteurs)
		TimeOutMoteur = TIMEOUTMOTEUR_SEC;
		// on indique qu'on a traité la consigne
		bflag_consigne_en_attente = false;
	}
	
	//cas ou on positionne le moteur à une position intermédiaire...
	// si la position (temps) est atteint
	if(bFlagMoteurPosOK == true){
		// on stoppe les moteurs
		digitalWrite(CMD_EXT, LOW);
		digitalWrite(CMD_PC, LOW);
		//DEBUG
		Serial.print("\n Position atteinte. Stop moteur !\n");

		// on reste le flag
		bFlagMoteurPosOK = false;
	}
}
	
//----------------------------------------------------------------------
//!\brief           INITIALISATION
//----------------------------------------------------------------------
void setup() {
	// init port COM du debug
	Serial.begin(57600);
	// affichage de la version
	Serial.println(version);
	Serial.println();
	Serial.println("Num. de Jeenode : ");
	Serial.println(stNum);
	
	//config des GPIO
	pinMode(CMD_EXT, OUTPUT);
	pinMode(CMD_PC, OUTPUT);
	pinMode(CMD_VMCDF, OUTPUT);
	pinMode(INFO_PC, INPUT);
	pinMode(INFO_EXT, INPUT);
	pinMode(LED, OUTPUT);
		
	// omit this call to avoid linking in the CRC calculation code
	hsensor.enableCRC();
	
	// Choix de la référence de tension pour le CAN interne
	//DEFAULT: the default analog reference of 3.3 volts
	// INTERNAL: an built-in reference, equal to 1.1 volts 
	// EXTERNAL: the voltage applied to the AREF pin is used as the reference.
	analogReference(DEFAULT);
	
	//initialisation du module radio RF12
	rf12_initialize(1, RF12_868MHZ, 33);
	// Reconfig du baudrate : 1200 bauds
	rf12_control(0xC6A3);	
	Serial.print("\nInit Radio : \n 1200 bauds\n");
	
	//Configuration d'un timer d'une seconde qui sert de scheduler
	MsTimer2::set(100, Timer100ms); // ms periode -> 1 seconde
	MsTimer2::start();

	Serial.println("\nFin SETUP");
	clignote_led();
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
//!\brief           BOUCLE PRINCIPAL
//----------------------------------------------------------------------
void loop() {

	tache_gestion_radio();
	
	tache_gestion_capteurs();
	
	tache_gestion_consignes();
	
	status();
	
	delay(10);
}
