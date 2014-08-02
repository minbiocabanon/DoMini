//--------------------------------------------------
//! \file     sketche_robomow.ino
//! \brief    Sketch pour le JeeNode : position GPS du robot tondeuse
//! \brief 	Les messages envoyes ont ce format:  $TDN,lat,lon     lat = latitude(°) x 10000  , lon(°)=longitude x 10000
//! \brief 	Le GPS doit avoir la config suivante : 4800-8-N-1 , messages NMEA0183 GGA RMC
//! \date     2012-06
//! \author   minbiocabanon
//--------------------------------------------------

#include <JeeLib.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <MsTimer2.h>
#include <math.h>
#include<stdlib.h>

#define NB_BLIP_LED	500		//nb de cycle a attendre entr 2 clignotements de la led status
#define version "JeeNode GPS Robomow"

// position de la base
#define BASE_LAT	43.791489		
#define BASE_LON	1.1077

// Mode radio
// Values 2 and 3 can cause the millisecond time to lose a few interrupts. 
// Value 3 can only be used if the ATmega fuses have been set for fast startup, i.e. 258 CK - the default Arduino fuse settings are not suitable for full power down.
#define MODE_RADIO_NORMAL 0
#define MODE_RADIO_IDLE 1
#define MODE_RADIO_STANDBY 2
#define MODE_RADIO_PWR_DOWN 3
#define RADIO_SLEEP 0
#define RADIO_WAKEUP -1


char stNum[] = "TND";
int nblipled;
boolean bflagTimeToGPS = false;

TinyGPS gps;
// declaratation d'un port COM serie software
SoftwareSerial ss(6, 11); //RX/TX

const int LED=14; 				//

//----------------------------------------------------------------------
//!\brief		Transmet les donne du buffer radio
//!\param	 	Buffer a envoyer
//!\return		?
//---------------------------------------------------------------------- 
void SendRadioData(float flat, float flon){
	// **************************************************
	// *	Compilation des donne dans un message
	// **************************************************
	
	// conversion du string en buffer pour la radio
	char payload[29]="";
	
	//conversion du float en INT x100
	long dflat = (float)flat * 1000000;
	long dflon = (float)flon * 1000000;
		
 	sprintf(payload,"$%s,%ld,%ld\r\n", stNum, dflat, dflon);
	

	// **************************************************
	// *	Emission des infos  sur la radio
	// **************************************************
	
	//DEBUG
	Serial.println("\nBuffer radio a emettre: ");
	for(byte i=0; i <= strlen(payload); i++)
		Serial.print(payload[i]);
	Serial.println();
	
	//A executer reliement 
	rf12_recvDone();
	
	//tant que porteuse pas libre
	while (!rf12_canSend())
		//on interroge le module
		rf12_recvDone();
	// porteuse libre, on envoie le message
	rf12_sendStart(0, payload, strlen(payload));
	Serial.print("\n\rMessage envoye OK\n\r");
	
	//attente de la fin d'ession radio
	rf12_sendWait(MODE_RADIO_IDLE);
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
//!\brief		Timer 1 seconde
//!\return		-
//---------------------------------------------------------------------- 
void Timer1seconde(void){

	bflagTimeToGPS = true;
}

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
//!\brief           INITIALISATION
//----------------------------------------------------------------------
void setup()
{
	Serial.begin(57600);
	ss.begin(4800);
  
	// affichage de la version
	Serial.println(version);
	Serial.println();
	Serial.println("Num. de Jeenode : ");
	Serial.println(stNum);
  
	Serial.print("Simple TinyGPS library v. "); Serial.println(TinyGPS::library_version());
	Serial.println("by Mikal Hart");
	Serial.println();
	
	//initialisation du module radio RF12
	rf12_initialize(1, RF12_868MHZ, 33);
	// Reconfig du baudrate : 1200 bauds
	rf12_control(0xC6A3);	
	Serial.print("\nInit Radio : \n 1200 bauds\n");

	
	//Configuration d'un timer d'une seconde qui sert de scheduler
	// MsTimer2::set(2500, Timer1seconde); // ms periode -> 1 seconde
	// MsTimer2::start();

	Serial.println("\nFin SETUP");
	clignote_led();
}

//----------------------------------------------------------------------
//!\brief           BOUCLE PRINCIPAL
//----------------------------------------------------------------------
void loop()
{
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 10000;)
  // if(bflagTimeToGPS == true)
  {
    while (ss.available())
    {
      char c = ss.read();
      // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
	bflagTimeToGPS = false;
  }

  if (newData)
  {
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    Serial.print("LAT=");
    Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    Serial.print(" LON=");
    Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    Serial.print(" SAT=");
    Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
    Serial.print(" PREC=");
    Serial.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
	Serial.println();
	
	SendRadioData(flat, flon);
	
	float distance_base = TinyGPS::distance_between(BASE_LAT, BASE_LON, flat, flon);
	Serial.print("distance BASE->Robot: ");
	Serial.println(distance_base,1);
	if(distance_base <= 20.0)
		Serial.println("Robot a la base");
	else if((distance_base >20.0) && (distance_base <= 80.0) )
		Serial.println("Robot sur le terrain");	
	else	
		Serial.println("Au voleur !!!!");
	  
	status();
   
	// gps.stats(&chars, &sentences, &failed);
	// Serial.print(" CHARS=");
	// Serial.print(chars);
	// Serial.print(" SENTENCES=");
	// Serial.print(sentences);
	// Serial.print(" CSUM ERR=");
	// Serial.println(failed);
    
 }
}
