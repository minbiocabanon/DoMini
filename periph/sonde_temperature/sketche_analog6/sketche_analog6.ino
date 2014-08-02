//--------------------------------------------------
//! \file     sketche_analog5.ino
//! \brief    Sketch pour le JeeNode : mesure température dans les combles
//! \date     2012-03
//! \author   minbiocabanon
//--------------------------------------------------

#include <JeeLib.h>
#include <MsTimer2.h>
#include <math.h>

#define version "JeeNode VMCDF+Bypass"
char stNum[] = "AN6";

#define NBMINUTEVEILLE	15

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
#define RTALON	20000.00
#define UALIM	3.3
#define R25		22000.00
#define B		3740
#define Q		3.3/1024

ISR(WDT_vect){Sleepy::watchdogEvent();}
//EMPTY_INTERRUPT(WDT_vect);

//prototype fonctions
void setup(void);
void CaptureSensor(void);
void SendRadioData(void);
void clignote_led(void);
void tache_gestion_led(void);
int read_temperature(int canal);

//variable capteurs SHT11
float h, t, d;
//variables pour la lecture du CAN interne
#define T_AIR_NEUF  	0		// Port1 AIO  - ANALOG 0 Selection de l'entree analog de mesure de la température air neuf	 envoyé dans la maison
#define T_AIR_EXT  		1		// Port2 AIO  - ANALOG 1 Selection de l'entree analog de mesure de la température air extérieur
#define T_AIR_REPRIS  	2		// Port2 AIO  - ANALOG 2 Selection de l'entree analog de mesure de la température air aspiré de la maison
#define T_AIR_EXTRAIT  	3		// Port2 AIO  - ANALOG 3 Selection de l'entree analog de mesure de la température air rejeté à l'extérieur

int byTemp_air_neuf;
int byTemp_air_ext;
int byTemp_air_repris;
int byTemp_air_extrait;


//flag
boolean bTimeToSendRadioMsg = false;
boolean bTimeToClignoteLed = false;

//chaine pour l'émission RF12
String str_payload = "";
//chaine pour l'émission RF12
char buffer_recept_rf12[66]="";

const int LED = 5; 				//Port2 DIO  - DIGITAL 5

//----------------------------------------------------------------------
//!\brief           fait clignoter la led une fois
//!\return        -
//----------------------------------------------------------------------
void clignote_led(void){
	int i = 0;
	for(i=0; i<2; i++){
		//allume la led
		digitalWrite(LED, LOW);
		delay(50);
		//eteint la led
		digitalWrite(LED, HIGH);
		delay(100);
	}
}

//----------------------------------------------------------------------
//!\brief		Fait l'acquisition des capteurs
//!\return		-
//---------------------------------------------------------------------- 
void CaptureSensor(void){

	byTemp_air_neuf = read_temperature(T_AIR_NEUF);
	byTemp_air_extrait = read_temperature(T_AIR_EXTRAIT);
	byTemp_air_repris = read_temperature(T_AIR_REPRIS);
	byTemp_air_ext = read_temperature(T_AIR_EXT);
		
}

//----------------------------------------------------------------------
//!\brief		Fait la conversion des données capteurs
//!\return		-
//---------------------------------------------------------------------- 
int read_temperature(int canal){
	
	//variable pour les valeurs analogiques
	double dbTensionVolt;
	int byTensionHEX = 0;  // variable to store the value coming from the sensor
	
	// **************************************************
	// *	Mesure de la temperature air neuf
	// **************************************************
	// Lecture de la valeur du CAN interne de l'ATmega
	byTensionHEX = analogRead(canal);
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
	return (dbTemp_air_neuf * 100);	
	
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
	char payload[]="$AN6,00000,00000,00000,00000**";
	
 	sprintf(payload,"$%s,%04d,%04d,%04d,%04d\r\n", stNum, byTemp_air_neuf, byTemp_air_extrait, byTemp_air_repris, byTemp_air_ext);
	
	// **************************************************
	// *	Emission des infos  sur la radio
	// **************************************************
	
	//DEBUG
	Serial.println("\nBuffer radio a emettre: ");
	for(byte i=0; i <= strlen(payload); i++)
		Serial.print(payload[i]);
	Serial.println();
	
	//réveil du module radio
	rf12_sleep(RADIO_WAKEUP);
		
	//tant que porteuse pas libre
	while (!rf12_canSend())
		//on interroge le module
		rf12_recvDone();
	// porteuse libre, on envoie le message
	rf12_sendStart(0, payload, strlen(payload));
	Serial.print("\n\rMessage envoye OK\n\r");
	
	//attente de la fin d'émission radio
	rf12_sendWait(MODE_RADIO_IDLE);

	// mise en veille (supposée) du module radio
	rf12_sleep(RADIO_SLEEP);
}

//----------------------------------------------------------------------
//!\brief          Tache qui gère l'envoi des messages avec les données capteurs
//!\param[in]    	-
//----------------------------------------------------------------------
void tache_gestion_capteurs(void){

	//DEBUG
	Serial.print("\nFlag IT TIMER !!!\n");
	// on capture les données des capteurs
	CaptureSensor();
	// on envoir les données vers la radio
	SendRadioData();
	// reset du flag timer
	bTimeToSendRadioMsg = false;
}

//----------------------------------------------------------------------
//!\brief          Fonction qui plonge le jeenode en veille
//!\param[in]    	--
//----------------------------------------------------------------------
void veille(void){
	unsigned int i=0;
	//Mise en veille du Jeenode, ATTENTION delay < 65 000
	for(i=0; i<NBMINUTEVEILLE ; i++)
		Sleepy::loseSomeTime(60000);
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
	
	pinMode(LED, OUTPUT);
			
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

	// mise en veille (supposée) du module radio
	rf12_sleep(RADIO_SLEEP);
	
	Serial.println("\nFin SETUP");
	
}

//----------------------------------------------------------------------
//!\brief           BOUCLE PRINCIPAL
//----------------------------------------------------------------------
void loop() {

	//on clignote la led
	clignote_led();
	
	// on capture la valeur des capteurs et on envoie un message radio
	tache_gestion_capteurs();
		
	// on passe en veille
	veille();
}
