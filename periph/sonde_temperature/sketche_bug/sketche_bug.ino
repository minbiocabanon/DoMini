//--------------------------------------------------
//! \file     sketche_analog5.ino
//! \brief    Sketch pour le JeeNode : mesure température dans les combles
//! \date     2012-03
//! \author   minbiocabanon
//--------------------------------------------------

#include <JeeLib.h>
#include <MsTimer2.h>

#define version "JeeNode VMCDF+Bypass"
char stNum[] = "AN9";

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

ISR(WDT_vect){Sleepy::watchdogEvent();}

//prototype fonctions
void setup(void);
void SendRadioData(void);
void clignote_led(void);
void Sleep(void);
const int LED=15; 				//Port2 AIO  - DIGITAL 15 

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
 	sprintf(payload,"$%s,0000,0000,0000,0000\r\n", stNum);

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
}



//----------------------------------------------------------------------
//!\brief          Fonction qui plonge le jeenode en veille
//!\param[in]    	--
//----------------------------------------------------------------------
void Sleep(void){
	unsigned int i=0;
	//Mise en veille du Jeenode, ATTENTION delay < 65 0000
	for(i=0; i<NBMINUTEVEILLE ; i++)
		Sleepy::loseSomeTime(60000);
}
	
//----------------------------------------------------------------------
//!\brief           INITIALISATION
//----------------------------------------------------------------------
void setup() {
	// init port COM du debug
	Serial.begin(57600);
	pinMode(LED, OUTPUT);
	//initialisation du module radio RF12
	rf12_initialize(1, RF12_868MHZ, 33);
	// Reconfig du baudrate : 1200 bauds
	rf12_control(0xC6A3);	
	Serial.print("\nInit Radio : \n 1200 bauds\n");
	// mise en veille (supposée) du module radio
	rf12_sleep(RADIO_SLEEP);	
}

//----------------------------------------------------------------------
//!\brief           BOUCLE PRINCIPAL
//----------------------------------------------------------------------
void loop() {
	//on clignote la led
	clignote_led();
	//send packet
	SendRadioData();
	// on passe en veille
	Sleep();
}
