// Sketch pour le JeeNode : acquisition des voies analogiques
// Transmission des données vers la radio
// Date de Création 2010-11-26

#include <JeeLib.h>
#include <math.h>

#define version "JeeNode Analog to RF12"
char stNum[] = "AN4";

#define NBMINUTEVEILLE	5

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
	

	
	// Choix de la référence de tension pour le CAN interne
	//DEFAULT: the default analog reference of 3.3 volts
	// INTERNAL: an built-in reference, equal to 1.1 volts 
	// EXTERNAL: the voltage applied to the AREF pin is used as the reference.
	analogReference(DEFAULT);
	
	//initialisation du module radio RF12
	rf12_initialize(1, RF12_868MHZ, 33);
	// Reconfig du baudrate : 1200 bauds
	rf12_control(0xC6A3);	
	Serial.print("\nInit Radio : \n 1200 bauds");
	
	// mise en veille (supposée) du module radio
	rf12_sleep(RADIO_SLEEP);

}

void loop() {
	
	//chaine pour l'émission RF12
	String str_payload = "";
	
	//variables pour la lecture du CAN interne
	int SENSORPIN1 = 0;    // select the input pin ADC0
	int SENSORPIN2 = 3;    // select the input pin ADC3
	int byTensionHEX1 = 0;  // variable to store the value coming from the sensor
	int byTensionHEX2 = 0;  // variable to store the value coming from the sensor
	
	//variable pour les valeurs analogiques
	double dbPyraVolt1;
	double dbPyraVolt2;
	
	//variable pour boucle
	int byi = 0;
	
	// attention 1 seconde pour stabiliser les tensions du CAN
	delay(1000);
	
	// **************************************************
	// *	Mesure de la tension pyranometre 1
	// **************************************************
	// Lecture de la valeur du CAN interne de l'ATmega
	for(byi=0; byi<7; byi++){
		//lecture + somme du CAN
		byTensionHEX1 += analogRead(SENSORPIN1);
		//délai de 200ms
		delay(200);
	}
	// on divise par 8 pour retrouver la valeur moyenne
	byTensionHEX1 = byTensionHEX1 / 8;
	
	//DEBUG
	// Serial.print("\nVolt HEXA. : ");
	// Serial.print(byTensionHEX,HEX);
	// Serial.print("-> DEC. : ");
	// Serial.print(byTensionHEX,DEC);
	
	dbPyraVolt1 = byTensionHEX1 * Q ;
	
	//DEBUG
	// Serial.print("\nVolt dbvolt. : ");
	// Serial.print(dbTensionVolt,4);
	//conversion du float en INT x100 , x100 pour supprimer la virgule et envoyer un entier
	int byPyraVolt1 = dbPyraVolt1 * 100;	
	//DEBUG
	Serial.print("\nbyPyraVolt1 : ");
	Serial.print(byPyraVolt1,DEC);
	
	// **************************************************
	// *	Mesure de la tension pyranometre 1
	// **************************************************
	// Lecture de la valeur du CAN interne de l'ATmega
	for(byi=0; byi<7; byi++){
		//lecture + somme du CAN
		byTensionHEX2 += analogRead(SENSORPIN2);
		//délai de 200ms
		delay(200);
	}
	// on divise par 8 pour retrouver la valeur moyenne
	byTensionHEX2 = byTensionHEX2 / 8;
	//DEBUG
	// Serial.print("\nVolt HEXA. : ");
	// Serial.print(byTensionHEX,HEX);
	// Serial.print("-> DEC. : ");
	// Serial.print(byTensionHEX,DEC);
	
	dbPyraVolt2 = byTensionHEX2 * Q ;
	
	//DEBUG
	// Serial.print("\nVolt dbvolt. : ");
	// Serial.print(dbTensionVolt,4);
	//conversion du float en INT x100 , x100 pour supprimer la virgule et envoyer un entier
	int byPyraVolt2 = dbPyraVolt2 * 100;	
	//DEBUG
	Serial.print("\nbyPyraVolt2 : ");
	Serial.print(byPyraVolt2,DEC);
		

	
	// **************************************************
	// *	Compilation des données dans un message
	// **************************************************
	
	//conversion du string en buffer pour la radio
	char payload[]="$AN1,1111,2222,3333,4444**";
	
 	sprintf(payload,"$%s,%04d,%04d,0000,0000\r\n", stNum, byPyraVolt1, byPyraVolt2);
	

	
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
	
	// Mise en veille du Jeenode, ATTENTION delay < 65 000
	for(byte i=0; i<NBMINUTEVEILLE ; i++)
		Sleepy::loseSomeTime(60000);
	 // delay(4000);
}
