// Sketch pour le JeeNode : acquisition des voies analogiques
// Transmission des donnees vers la radio
// Date de Creation 2011-12-10

#include <JeeLib.h>
#include <math.h>


#define version "JeeNode CONGELO"
char stNum[] = "AN4";
#define LED_PIN 9

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

//Pour calcul temperature
#define B 3740
#define Rtalon 24000
#define Ualim 3.25
#define R25 22000
#define q (Ualim / 1024) // 3.25 / 2^10


int sensorPin = A1;    // select the input pin for the potentiometer
int sensorValue = 0;  // variable to store the value coming from the sensor

Port port (1);

ISR(WDT_vect){Sleepy::watchdogEvent();}


void setup() {
	// init port COM du debug
	Serial.begin(57600);
	// affichage de la version
	Serial.println(version);
	Serial.println();
	Serial.println("N° de Jeenode : ");
	Serial.println(stNum);
	
	//initialisation de la sortie qui pilote la led
	pinMode(LED_PIN, OUTPUT);
	//on eteind la led
	digitalWrite(LED_PIN, 1);       
	
	// Choix de la reference de tension pour le CAN internet
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
	//rf12_sleep(RADIO_SLEEP);

}

void loop() {
	unsigned long val1,val2,val3,val4;
	double Temp,U;
	
	Serial.print("\nInterne A0 - ");
	// Lecture de la valeur du CAN interne a l'ATmega
	//sensorValue = analogRead(sensorPin); 
	sensorValue = port.anaRead();
	Serial.print(sensorValue);
	U = sensorValue * q ;
	Serial.print("\nU : ");
	Serial.print(U,2);
	Serial.println();
	
	Temp = -273.15 + 1/((log((U*Rtalon/(Ualim-U))/R25))/B +(1/(273.15+25)));
	Serial.print("\nTemp1 : ");
	Serial.print(Temp,2);
	Serial.println();
	

	
	// **************************************************
	// *    Compilation des donnees dans un message
	// **************************************************
	
	//on allume la led
	digitalWrite(LED_PIN, 0);
	
	// conversion du string en buffer pour la radio
	char payload[]="$AN4,-1111,-2222,-3333,-4444**";
	
	//conversion du FLOAT en speudo FLOAT   ,  * 100 pour 2 chiffre apres la virgule. pour 3 chiffre "* 1000"...
	//long temp1 = (Temp - (long)Temp) * 100;

	//conversion du float en INT x100
	int temp1 = Temp * 100;

	//sprintf(payload,"$AN1,%0ld.%0ld,%0ld.%0ld,%0ld.%0ld,%0ld.%0ld\r\n", (long)Temp, temp1, (long)Temp, temp1, (long)Temp, temp1, (long)Temp, temp1);
	sprintf(payload,"$%s,%04d,0000,0000,0000\r\n", stNum, temp1);
        

        
    // **************************************************
    // *    Emission des infos  sur la radio
    // **************************************************
	//DEBUG
	Serial.println("\nBuffer radio a emettre: ");
	for(byte i=0; i <= strlen(payload); i++)
		Serial.print(payload[i]);
	Serial.println();

	//reveil du module radio
	//rf12_sleep(RADIO_WAKEUP);
	
	//A executer regulierement 
	//rf12_recvDone();
	
	//tant que porteuse pas libre
	while (!rf12_canSend())
		//on interroge le module
		rf12_recvDone();
	// porteuse libre, on envoie le message
	rf12_sendStart(0, payload, strlen(payload));
	Serial.print("\n\rMessage envoye OK\n\r");
	
	//attente de la fin d'emission radio
	rf12_sendWait(MODE_RADIO_IDLE);

	// mise en veille (supposee) du module radio
	//rf12_sleep(RADIO_SLEEP);
	
	//Mise en veille du Jeenode, ATTENTION delay < 65 0000
	for(byte i=0; i<NBMINUTEVEILLE ; i++)
		Sleepy::loseSomeTime(60000);
	//on eteind la led
	digitalWrite(LED_PIN, 1); 
	 // delay(2000);
}

