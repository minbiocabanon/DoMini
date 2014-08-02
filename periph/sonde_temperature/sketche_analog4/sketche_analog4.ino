//--------------------------------------------------
//! \file     sketche_analog4.ino
//! \brief    Sketch pour le JeeNode : mesure température sortie bouche insuflation d'une piece
//! \date     2012-03
//! \author   minbiocabanon
//--------------------------------------------------

#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <JeeLib.h>
#include <math.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define version "JeeNode VMCDF+Bypass"
char stNum[] = "AN4";

#define NBMINUTEVEILLE	5
#define CFG_WDT			9 	// 0=16ms, 1=32ms, 2=64ms, 3=128ms, 4=250ms, 5=500ms, 6=1 sec, 7=2 sec, 8=4 sec, 9= 8sec

// Mode radio
// Values 2 and 3 can cause the millisecond time to lose a few interrupts. 
// Value 3 can only be used if the ATmega fuses have been set for fast startup, i.e. 258 CK - the default Arduino fuse settings are not suitable for full power down.
#define MODE_RADIO_NORMAL 0
#define MODE_RADIO_IDLE 1
#define MODE_RADIO_STANDBY 2
#define MODE_RADIO_PWR_DOWN 3
#define RADIO_SLEEP 0
#define RADIO_WAKEUP -1

//Constante pour la conversion tension -> °C
#define RTALON	20000.00
#define UALIM	3.3
#define R25		22000.00
#define B		3740
#define Q		(UALIM / 1024) // 3.25 / 2^10

volatile boolean f_wdt=1;



//prototype fonctions
void setup(void);
void CaptureSensor(void);
void SendRadioData(void);


//variable capteurs SHT11
float h, t, d;
//variables pour la lecture du CAN interne
#define T_AIR_NEUF  	3		// Port4 AIO  - ANALOG 3 Selection de l'entree analog de mesure de la température air neuf	 
int byTemp_air_vicie;
int byTemp_air_neuf;

//flag
boolean bTimeToSendRadioMsg = false;
boolean bTimeToClignoteLed = false;

unsigned long TicksMsgRad = 0;
unsigned long TicksLed = 0;

//chaine pour l'émission RF12
String str_payload = "";
//chaine pour l'émission RF12
char buffer_recept_rf12[66]="";

const int LED=15; 				//Port2 AIO  - DIGITAL 15 

//----------------------------------------------------------------------
//!\brief           Watchdog Interrupt Service / is executed when  watchdog timed out
//!\return        -
//----------------------------------------------------------------------
ISR(WDT_vect){
	f_wdt=1;  // set global flag
}

//----------------------------------------------------------------------
//!\brief		Fait l'acquisition des capteurs
//!\return		-
//---------------------------------------------------------------------- 
void CaptureSensor(void){
	
	//variable pour les valeurs analogiques
	double dbTensionVolt = 0, dbTemp = 0;
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
	//DEBUG
	// Serial.print("\nVolt dbvolt. : ");
	// Serial.print(dbTensionVolt,4);
	
	dbTemp = -273.15 + 1 / ( (log( (dbTensionVolt * RTALON / (UALIM - dbTensionVolt) ) / R25) )/ B + ( 1 / (273.15 + 25)));
	
	//conversion du float en INT x100 , x100 pour supprimer la virgule et envoyer un entier
	byTemp_air_neuf = dbTemp * 100;	
	//DEBUG
	// Serial.print("\nTemperature. : ");
	// Serial.print(byTemp_air_neuf,DEC);
}

//----------------------------------------------------------------------
//!\brief		Transmet les données du buffer radio
//!\param	 	-
//!\return		-
//---------------------------------------------------------------------- 
void SendRadioData(void){
	// **************************************************
	// *	Compilation des données dans un message
	// **************************************************
	
	//réveil du module radio
	rf12_sleep(RF12_WAKEUP);
	
	// conversion du string en buffer pour la radio
	char payload[29]="";
	
 	sprintf(payload,"$%s,%04d,0000,0000,0000\r\n", stNum, byTemp_air_neuf);
	

	// **************************************************
	// *	Emission des infos  sur la radio
	// **************************************************
	
	//DEBUG
	// Serial.println("\nBuffer radio a emettre: ");
	// for(byte i=0; i <= strlen(payload); i++)
		// Serial.print(payload[i]);
	// Serial.println();
	
	//A executer régulièrement 
	rf12_recvDone();
	
	//tant que porteuse pas libre
	while (!rf12_canSend())
		//on interroge le module
		rf12_recvDone();
	// porteuse libre, on envoie le message
	rf12_sendStart(0, payload, strlen(payload));

	// attente de la fin d'émission radio
	rf12_sendWait(3);
	// mise en veille (supposée) du module radio
	rf12_sleep(RF12_SLEEP);
	// Serial.print("\n\rMessage envoye OK\n\r");
}

//----------------------------------------------------------------------
//!\brief		Mise en veille profonde pendant plusieurs minutes
//!\param	 	-
//!\return		-
//---------------------------------------------------------------------- 
void deep_sleep(void){
	//Mise en veille du Jeenode
	// Pour CFG_WDT = 9, watchdog claque toutes les 8192ms. 
	// Pour 5 minutes de période, i = 5 * 60 * 1000 / 8192 = 37.7
	// Pour 15 minutes de période, i = 15 * 60 * 1000 / 8192 = 109.8
	for(int i = 0; i < 110 ; i++){
		//on passe en veille profonde
		system_sleep();
	}
}


//----------------------------------------------------------------------
//!\brief          set system into the sleep state 
//!\brief          system wakes up when wtchdog is timed out
//!\param[in]    	-
//----------------------------------------------------------------------
void system_sleep() {
	cbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter OFF
	set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
	sleep_enable();
	sleep_mode();                        // System sleeps here
	sleep_disable();                     // System continues execution here when watchdog timed out 
	sbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter ON
}

//----------------------------------------------------------------------
//!\brief          0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms 
//!\brief          6=1 sec,7=2 sec, 8=4 sec, 9= 8sec
//!\param[in]    	-
//----------------------------------------------------------------------
void setup_watchdog(int ii) {

  byte bb;
  int ww;
  if (ii > 9 ) ii=9;
  bb=ii & 7;
  if (ii > 7) bb|= (1<<5);
  bb|= (1<<WDCE);
  ww=bb;
  Serial.println(ww);

  MCUSR &= ~(1<<WDRF);
  // start timed sequence
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  // set new watchdog timeout value
  WDTCSR = bb;
  WDTCSR |= _BV(WDIE);
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
	// see http://tools.jeelabs.org/rfm12b
    rf12_control(0xC040); // set low-battery level to 2.2V i.s.o. 3.1V

	
	// mise en veille (supposée) du module radio
	rf12_sleep(RF12_SLEEP);
	
	// CPU Sleep Modes 
	// SM2 SM1 SM0 Sleep Mode
	// 0    0  0 Idle
	// 0    0  1 ADC Noise Reduction
	// 0    1  0 Power-down
	// 0    1  1 Power-save
	// 1    0  0 Reserved
	// 1    0  1 Reserved
	// 1    1  0 Standby(1)

	cbi( SMCR,SE );      // sleep enable, power down mode
	cbi( SMCR,SM0 );     // power down mode
	sbi( SMCR,SM1 );     // power down mode
	cbi( SMCR,SM2 );     // power down mode

	setup_watchdog(CFG_WDT);

	Serial.println("\nFin SETUP");
	
	//on met le flag à 1 pour envoyer un premier message au démarrage du module
	bTimeToSendRadioMsg = true;
}

//----------------------------------------------------------------------
//!\brief           BOUCLE PRINCIPAL
//----------------------------------------------------------------------
void loop() {
	// on capture les données des capteurs
	CaptureSensor();
	// on envoie les données vers la radio
	SendRadioData();
	//Mise en veille profonde
	deep_sleep();
		
}
