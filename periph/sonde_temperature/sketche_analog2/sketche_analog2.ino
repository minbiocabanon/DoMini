// Sketch pour le JeeNode : acquisition des voies analogiques
// Transmission des données vers la radio

#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <JeeLib.h>
#include <PortsSHT11.h>
#include <math.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define version "JeeNode Analog to RF12"
char stNum[] = "AN2";

#define NBMINUTEVEILLE	5
#define CFG_WDT			9 	// 0=16ms, 1=32ms, 2=64ms, 3=128ms, 4=250ms, 5=500ms, 6=1 sec, 7=2 sec, 8=4 sec, 9= 8sec
// Mode radio
// Values 2 and 3 can cause the millisecond time to lose a few interrupts. 
// Value 3 can only be used if the ATmega fuses have been set for fast startup, i.e. 258 CK - the default Arduino fuse settings are not suitable for full power down.
#define MODE_RADIO_NORMAL 0
#define MODE_RADIO_IDLE 1
#define MODE_RADIO_STANDBY 2
#define MODE_RADIO_PWR_DOWN 3

#define Q 3.3/1024

//Declation des variables pour les GPIO
#define CMD_VDD  7              // Commande du transistor pour avoir VDD = VCC (alim pyranomètre + pont diviseur alim) PD7 - Pin13 - Digital 7


volatile boolean f_wdt=1;

//Declaration du capteur SHT11
SHT11 hsensor (3);

// Watchdog Interrupt Service / is executed when  watchdog timed out
ISR(WDT_vect){
	f_wdt=1;  // set global flag
}

void setup() {
	// init port COM du debug
	Serial.begin(57600);
	// affichage de la version
	Serial.println(version);
	Serial.println();
	Serial.println("N° de Jeenode : ");
	Serial.println(stNum);
	
	pinMode(CMD_VDD, OUTPUT);
	
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
}

void loop() {
	
	//chaine pour l'émission RF12
	String str_payload = "";
	
	//variables pour la lecture du CAN interne
	int SENSORPIN1 = 0;    // select the input pin ADC0 - Tension batterie
	int SENSORPIN2 = 3;    // select the input pin ADC3 - Tension pyranometre
	
	int byTensionHEX1 = 0;  // variable to store the value coming from the sensor
	int byTensionHEX2 = 0;  // variable to store the value coming from the sensor
	
	//variable pour les valeurs analogiques
	double dbTensionVolt;
	double dbPyraVolt2;
	
	//variable pour boucle de filtrage
	int byi = 0;
	
	// **************************************************
	// *	Mesure de la tension batterie
	// **************************************************
	//on active l'alim avec une mise à la masse sur la grille du PMOS
	digitalWrite(CMD_VDD, LOW);
	//délai de 50ms
	delay(50);

	// Lecture de la valeur du CAN interne de l'ATmega
	byTensionHEX1 = analogRead(SENSORPIN1);
	
	//on désactive l'alim avec une mise à la masse sur la grille du PMOS
	digitalWrite(CMD_VDD, HIGH);
	
	//DEBUG
	// Serial.print("\nVolt HEXA. : ");
	// Serial.print(byTensionHEX,HEX);
	// Serial.print("-> DEC. : ");
	// Serial.print(byTensionHEX,DEC);
	
	dbTensionVolt = byTensionHEX1 * Q * 2 ; //x2 pour le pont diviseur
	
	//DEBUG
	// Serial.print("\nVolt dbvolt. : ");
	// Serial.print(dbTensionVolt,4);
	//conversion du float en INT x100 , x100 pour supprimer la virgule et envoyer un entier
	int byTensionVolt = dbTensionVolt * 100;	
	//DEBUG
	// Serial.print("\nVolt byvolt. : ");
	// Serial.print(byTensionVolt,DEC);
		
	// **************************************************
	// *	Lecture des données dans le capteur
	// **************************************************
	
	uint8_t error = hsensor.measure(SHT11::HUMI);        
	error |= hsensor.measure(SHT11::TEMP);
	// Serial.print(hsensor.meas[SHT11::TEMP]);
	// Serial.print(error, DEC);

	// omit following code to avoid linking in floating point code
	float h, t;
	hsensor.calculate(h, t);
	// Serial.print("\nHumid. : ");
	// Serial.print(h);
	// Serial.print("%\nTemp. :");
	// Serial.print(t);
		
	// omit following code to avoid linking in the log() math code
	float d = hsensor.dewpoint(h, t);
	// Serial.print("C\nPoint de rosee :");
	// Serial.print(d);
	// Serial.print("C\n\n");
	
	// **************************************************
	// *	Mesure de la tension pyranometre 1
	// **************************************************
	// Lecture de la valeur du CAN interne de l'ATmega
	byTensionHEX2 = analogRead(SENSORPIN2);
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
	// Serial.print("\nbyPyraVolt2 : ");
	// Serial.print(byPyraVolt2,DEC);
	
	// **************************************************
	// *	Compilation des données dans un message
	// **************************************************
	
	// conversion du string en buffer pour la radio
	//char payload[]="$AN1,1111,2222,3333,4444,5555,6666**";
    char payload[]="$AN5,-1111,-2222,-3333,-4444**";
	
	//conversion du float en INT x100
	long temp = t * 100;
	int humid = h * 100;
	int dewpt = d * 100;
 	//sprintf(payload,"$%s,%04ld,%04d,%04d,%04d,%04d,0000\r\n", stNum, (long)temp, humid, dewpt, byTensionVolt, byPyraVolt2);
	sprintf(payload,"$%s,%04ld,%04d,%04d,%04d\r\n", stNum, (long)temp, humid, dewpt, byTensionVolt);
	
		
	// **************************************************
	// *	Emission des infos  sur la radio
	// **************************************************
	
	//DEBUG
	// Serial.println("\nBuffer radio a emettre: ");
	// for(byte i=0; i <= strlen(payload); i++)
		// Serial.print(payload[i]);
	// Serial.println();

	//réveil du module radio
	rf12_sleep(RF12_WAKEUP);
	
	//A executer régulièrement 
	rf12_recvDone();
	
	//tant que porteuse pas libre
	while (!rf12_canSend())
		//on interroge le module
		rf12_recvDone();
	// porteuse libre, on envoie le message
	rf12_sendStart(0, payload, strlen(payload));
	// Serial.print("\n\rMessage envoye OK\n\r");
	
	// attente de la fin d'émission radio
    // set the sync mode to 2 if the fuses are still the Arduino default
    // mode 3 (full powerdown) can only be used with 258 CK startup fuses

	// attente de la fin d'émission radio
	rf12_sendWait(3);
	// mise en veille (supposée) du module radio
	rf12_sleep(RF12_SLEEP);
	// Serial.print("\n\rMessage envoye OK\n\r");

	//Mise en veille du Jeenode
	// Pour CFG_WDT = 9, watchdog claque toutes les 8192ms. 
	// Pour 5 minutes de période, i = 5 * 60 * 1000 / 8192 = 37.7
	for(int i = 0; i < 38 ; i++){
		//on passe en veille profonde
		system_sleep();
	}
	// on loop !
}

//****************************************************************  
// set system into the sleep state 
// system wakes up when wtchdog is timed out
void system_sleep() {
	cbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter OFF
	set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
	sleep_enable();
	sleep_mode();                        // System sleeps here
	sleep_disable();                     // System continues execution here when watchdog timed out 
	sbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter ON
}

//****************************************************************
// 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
// 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec
void setup_watchdog(int time) {

  byte bmasque;
  
  if (time > 9 )
	time=9;
  bmasque=time & 7;
  
  if (time > 7) 
	bmasque|= (1<<5);
  
  bmasque|= (1<<WDCE);
  
  MCUSR &= ~(1<<WDRF);
  // start timed sequence
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  // set new watchdog timeout value
  WDTCSR = bmasque;
  WDTCSR |= _BV(WDIE);
}