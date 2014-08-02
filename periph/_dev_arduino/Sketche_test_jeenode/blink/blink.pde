// Sketch pour le JeeNode : gestion de la VMC et du bypass
// Transmission des données vers la radio
// Date de Création 2011-02-17


#include <Ports.h>
#include "PortsSHT11.h"
#include <RF12.h>
#include <RF12sio.h>
#include <MsTimer2.h>
#include <math.h>


#define version "JeeNode VMCDF+Bypass by RzBo-Jenipapo V1.0 02/17/2011"
char stNum[] = "TEST clignotage led sur GPIO6 et GPIO13";

#define ledPin6 4                 // LED connectée à la broche numérique 6
#define ledPin13 7                 // LED connectée à la broche numérique 13
#define LED_PIN 9


// **************************************************
// *	SETUP
// **************************************************
void setup() {
	// init port COM du debug
	Serial.begin(57600);
	// affichage de la version
	Serial.println(version);
	Serial.println();
	Serial.println("Num. de Jeenode : ");
	Serial.println(stNum);
	
	//config des GPIO
	pinMode(ledPin6, OUTPUT);      // met la broche numérique en sortie 
	pinMode(ledPin13, OUTPUT);      // met la broche numérique en sortie 
	pinMode(LED_PIN, OUTPUT);

}

// **************************************************
// *	LOOP
// **************************************************


void loop()
{
  digitalWrite(ledPin6, 1);   // allume la LED
  digitalWrite(ledPin13, 0);    // éteint la LED
  digitalWrite(LED_PIN, 1);
  delay(1000);                  // attend une seconde
  Serial.println("loop1");
  digitalWrite(ledPin13, 1);   // allume la LED
  digitalWrite(ledPin6, 0);    // éteint la LED
  digitalWrite(LED_PIN, 0);
  delay(1000);                  // attend une seconde
  Serial.println("loop2");
}