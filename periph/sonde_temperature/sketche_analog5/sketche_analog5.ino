//--------------------------------------------------
//! \file     sketche_analog5.ino
//! \brief    Sketch pour le JeeNode : mesure température dans les combles
//! \date     2010-11-26
//! \author   minbiocabanon
//--------------------------------------------------

#include <JeeLib.h>
#include <PortsSHT11.h>
#include <math.h>

#define version "JeeNode Analog to RF12 RzBo-Jenipapo V2.0 09/01/2011"
char stNum[] = "AN5";

#define NBMINUTEVEILLE	30

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
#define Q		(UALIM / 1024) // 3.25 / 2^10

//variables pour la lecture du CAN interne
#define T_AIR_NEUF  	0		// Port1 AIO  - ANALOG 0 Selection de l'entree analog de mesure de la température air neuf	 
int byTemp_air_neuf;

ISR(WDT_vect){Sleepy::watchdogEvent();}

void setup() {
	// init port COM du debug
	Serial.begin(57600);
	// affichage de la version
	Serial.println(version);
	Serial.println();
	Serial.println("N° de Jeenode : ");
	Serial.println(stNum);
	
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

}

void loop() {
	
	
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
	// *	Compilation des données dans un message
	// **************************************************
	
	// conversion du string en buffer pour la radio
	char payload[]="$AN5,-1111,-2222,-3333,-4444**";
	sprintf(payload,"$%s,%04d,0000,0000,0000\r\n", stNum, byTemp_air_neuf);
	
		
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

	// mise en veille (supposée) du module radio
	rf12_sleep(RADIO_SLEEP);
	
	//Mise en veille du Jeenode, ATTENTION delay < 65 0000
	for(byte i=0; i<NBMINUTEVEILLE ; i++)
		Sleepy::loseSomeTime(60000);
	 // delay(2000);
}
