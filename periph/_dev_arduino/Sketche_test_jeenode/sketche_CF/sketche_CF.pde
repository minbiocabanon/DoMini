// Sketch pour le JeeNode : acquisition des voies analogiques
// Transmission des données vers la radio
// Date de Création 2010-11-26
// Mise à jour : 09/01/2011 -> remplacement CTN + CAN I2C par le capteur SHT11 I2C , donnant T° + H° + point de rosee
// Mise à jour : 12/01/2011 -> ajout mode low power
// Mise à jour :13/01/2011 -> ajout info tension de la pile


#include <Ports.h>
#include "PortsSHT11.h"
#include <RF12.h>
#include <RF12sio.h>
#include <math.h>


#define version "JeeNode Analog to RF12 RzBo-Jenipapo V2.0 09/01/2011"
char stNum[] = "AN1";
char byCpt = 0;

#define NBMINUTEVEILLE	1

// Mode radio
// Values 2 and 3 can cause the millisecond time to lose a few interrupts. 
// Value 3 can only be used if the ATmega fuses have been set for fast startup, i.e. 258 CK - the default Arduino fuse settings are not suitable for full power down.
#define MODE_RADIO_NORMAL 0
#define MODE_RADIO_IDLE 1
#define MODE_RADIO_STANDBY 2
#define MODE_RADIO_PWR_DOWN 3
#define RADIO_SLEEP 0
#define RADIO_WAKEUP -1

#define Q 3.3/1024

ISR(WDT_vect){Sleepy::watchdogEvent();}

void setup() {
	// init port COM du debug
	Serial.begin(57600);
	// affichage de la version
	Serial.println(version);
	Serial.println();
	Serial.println("N° de Jeenode : ");
	Serial.println(stNum);
	
	
	//initialisation du module radio RF12
	rf12_initialize(2, RF12_868MHZ, 33);
	// Reconfig du baudrate : 1200 bauds
	rf12_control(0xC6A3);	
	Serial.print("\nInit Radio : \n 1200 bauds");
	
	// mise en veille (supposée) du module radio
	rf12_sleep(RADIO_SLEEP);

}

void loop() {
	

	// **************************************************
	// *	Compilation des données dans un message
	// **************************************************
	
	// conversion du string en buffer pour la radio
	char payload[29]="";
	char stpayload[] ="$ T'as recu le msg num"; 
	
	byCpt++;
	
 	sprintf(payload,"%s %d \n", stpayload, byCpt);
	

	
	// **************************************************
	// *	Emission des infos  sur la radio
	// **************************************************
	
	//DEBUG
	Serial.println("\nBuffer radio a emettre: ");
	for(byte i=0; i <= sizeof payload; i++)
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
	rf12_sendStart(0, payload, sizeof payload);
	Serial.print("\n\rMessage envoye OK\n\r");
	
	//attente de la fin d'émission radio
	rf12_sendWait(MODE_RADIO_IDLE);

	// mise en veille (supposée) du module radio
	rf12_sleep(RADIO_SLEEP);
	
	//Mise en veille du Jeenode, ATTENTION delay < 65 0000
	// for(byte i=0; i<NBMINUTEVEILLE ; i++)
		// Sleepy::loseSomeTime(60000);
	 delay(2000);
}