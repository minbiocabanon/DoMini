====== Introduction ======

Cette page présente le shield que j'ai réalisé pour les modules [[http://jeelabs.com/products/jeenode|Jeenode]] fabriqués par [[http://jeelabs.org/| JeeLabs]].

Les Jeenodes sont '[[http://www.arduino.cc/|arduino]] compatibles'.


====== Description ======

Les fonctions apportées par ce shield sont les suivantes :

  * Convertisseur DC/DC autorisant une tension d'alimentation de 0.9V - 5.5V : validée pour fonctionner sur un accu ou une pile, mode basse conso en veille. //Attention, modification à réaliser sur le Jeenode, cf. §[[wiki_sur_le_shield_capteurs_jeenode#Hardware|Hardware]]//
  * Capteur température et humidité I²C (SHT11 de Sensirion)
  * Mesure de tension de la pile/batterie
  * 1 interface pour piloter une BPW34 (pyranomètre). //Peut être convertie en sortie pour fournir une alimentation//
  * 1 sortie collecteur ouvert (mosfet)
  * 1 CTN 22K (interne ou externe sur connecteur)
  * 1 Led


====== Hardware ======

===== Schéma =====

{{:ci13-001-schematic.png |}}


Lien vers le schéma au format PDF : {{::schematic-jeenode.pdf|}}

===== Nomenclature =====
[[BOM-CI13-001]]

===== Modifications à réaliser sur le jeenode =====

Dans le cas d'une utilisation du Jeenode en mode "low power", il faut enlever le régulateur de tension IC2 afin de minimiser la consommation totale de la carte.
L'inconvénient sera de devoir laisser la batterie (ou autre) connectée lorsque l'on voudra brancher le Jeenode sur le USB-BUB lors d'une reprogrammation. :-\

//(photo)//

===== Ressources utilisées =====
==== P1 ====
  * P1.5 - AIO1 : Mesure de tension batterie
  * P1.2 - DIO1 : Etat led. 0 = led allumée

==== P2 ====
  * P2.5 -  AIO2 : Mesure de température CTN
  * P2.2 -  DIO2 : Commande transistor mosfet (spare)

==== P3 ====
  * P3.5 -  SCL: Signal I2C Clock
  * P3.2 -  SDA : Signal I2C Data

==== P4 ====
  * P4.5 -  AIO4 : Mesure Analogique pyranomètre
  * P4.2 -  DIO4 : Pilotage polarisation de la diode pyranomètre (BPW34)

===== PCB =====
{{::ci13-001-bottomview.png?200|}}
{{::ci13-001-topview.png?200|}}

===== Implantation =====

Fichier pdf avec l'implantation et les valeurs des composants : {{::ci13-001_implantation.pdf|}}

{{ ::20140119_pcb_arduino_004.jpg?400 |}}
===== Composants conseillés =====

  * Pyranomètre : référence de la photodiode : {{::bpw34_photodiode.pdf|BPW34}}. Il faut la passe entre P5.1 et P5.2. La cathode de la photodiode sur P5.2 .
  * Capteur I²C : le circuit est adapté pour un {{::sht11_humidistancei2c.pdf|SHT11}} de Sensirion. La librairie est disponible sur arduino.



====== Software ======
===== Généralités =====

==== IDE ====


==== Librairie arduino ====


==== Librairie Jeenode ====

Schéma qui illustre les ressources et correspondances avec les dénominations dans les librairies :\\

{{:qrc-jn-v6-part.png?200|}}
{{:legend1.png?100|}}

\\ source : [[http://jeelabs.org/2013/03/22/jeenode-v6-reference/]]
==== Nota ====
//Nota : attention, la transmission de nombre flottant via le module radio RFM12 n'est pas supportée. Il faut convertir en nombre entier (x100 ou x1000 selon la précision souhaitée) puis diviser côté réception.
//

===== Led =====
Exemple de l'utilisation de la led, câblée sur la sortie DIO1 (Port 1).
Le code suivant fait clignoter la led : 500ms ON puis 1s OFF.


<code c>
#include <JeeLib.h>
const int LED = 4; 				//Port1 DIO  - DIGITAL 4 
 
void setup () {
  // init port COM debug
  Serial.begin(57600);
}
 
void loop () {
  //led OFF
  digitalWrite(LED, HIGH);
  delay(500);
  //led ON
  digitalWrite(LED, LOW);
  delay(1000);
}
</code>


===== Capteur SHT11 =====

Exemple de code pour lire les infos du capteur SHT11 : 

<code c>
#include <JeeLib.h>
#include <PortsSHT11.h>

//Declaration du capteur SHT11 , connecté au port 3
SHT11 hsensor (3);

void setup () {
	// init port COM du debug
	Serial.begin(57600);
	// omit this call to avoid linking in the CRC calculation code
	hsensor.enableCRC();
}

void loop () {
	// **************************************************
	// *	Lecture des données dans le capteur SHT11
	// **************************************************
	
	uint8_t error = hsensor.measure(SHT11::HUMI);        
	error |= hsensor.measure(SHT11::TEMP);
	// Serial.print(hsensor.meas[SHT11::TEMP]);
	// Serial.print(error, DEC);

	// omit following code to avoid linking in floating point code
	float h, t;
	hsensor.calculate(h, t);
	Serial.print("\nHumid. : ");
	Serial.print(h);
	Serial.print("%\nTemp. :");
	Serial.print(t);
		
	// omit following code to avoid linking in the log() math code
	float d = hsensor.dewpoint(h, t);
	Serial.print("C\nPoint de rosee :");
	Serial.print(d);
	Serial.print("C\n\n");

	delay(2000);
}
</code>

Sur le terminal de l'arduino IDE, les lignes suivantes s'affichent chaque seconde :
<code>
Humid. : 66.06%
Temp. :22.57C
Point de rosee :15.91C
</code>
===== Lecture entrée analogie (tension batterie )=====

La tension batterie se lit sur l'entrée ADC0


<code c>
#include <JeeLib.h>

#define Q 3.3/1024

void setup () {
	// init port COM du debug
	Serial.begin(57600);

	// Choix de la référence de tension pour le CAN interne
	//DEFAULT: the default analog reference of 3.3 volts
	// INTERNAL: an built-in reference, equal to 1.1 volts 
	// EXTERNAL: the voltage applied to the AREF pin is used as the reference.
	analogReference(DEFAULT);
}

void loop () {
	//variables pour la lecture du CAN interne
	int SENSORPIN1 = 0;    // select the input pin ADC0 - Tension batterie
	//variable pour les valeurs analogiques
	double dbTensionVolt;

	// **************************************************
	// *	Mesure de la tension batterie
	// **************************************************
	// Lecture de la valeur du CAN interne de l'ATmega
	dbTensionVolt = analogRead(SENSORPIN1) * Q * 2; //x2.00 pour le pont diviseur
	
	//DEBUG
	Serial.print("\nVolt dbvolt. : ");
	Serial.print(dbTensionVolt,4);

	delay(2000);
}
</code>

===== Lecture entrée analogie (pyranomètre )=====

La tension pyranomètre se lit sur l'entrée ADC3.
Attention, pour économiser l'énergie la photodiode est polarisée à travers un transistor MOS. Il faut piloter le transistor pour polarisé la photodiode, réaliser la mesure puis bloquer le transistor.
Cette opération n'est pas obligatoire si le module n'est pas alimenté sur pile/batterie.

''Attention : le sketche suivant mesure la tension présente sur le potentiel "batterie"''


<code c>
#include <JeeLib.h>
//Declation des variables pour les GPIO
#define CMD_BPW34  7              // Commande transistor sur PD7 - Pin13

#define Q 3.3/1024

void setup () {
	// init port COM du debug
	Serial.begin(57600);

	// Choix de la référence de tension pour le CAN interne
	//DEFAULT: the default analog reference of 3.3 volts
	// INTERNAL: an built-in reference, equal to 1.1 volts 
	// EXTERNAL: the voltage applied to the AREF pin is used as the reference.
	analogReference(DEFAULT);
}

void loop () {
	//variables pour la lecture du CAN interne
	int SENSORPIN2 = 3;    // select the input pin ADC3 - Tension pyranometre
	//variable pour les valeurs analogiques
	double dbPyraVolt2;

	// **************************************************
	// *	Mesure de la tension pyranometre 1
	// **************************************************
	
	//on active l'alim de la BPW34 avec une mise à la masse sur la grille du PMOS
	digitalWrite(CMD_BPW34, LOW);
	//délai de 50ms
	delay(50);
		
	//on faite une boucle pour mesurer la valeur du pyranomètre
	// Lecture de la valeur du CAN interne de l'ATmega
	//conversion du float en INT x100 , x100 pour supprimer la virgule et envoyer un entier
	int byPyraVolt2 = analogRead(SENSORPIN2) * Q * 100;

	//on désactive l'alim de la BPW34 avec une mise à VCC sur la grille du PMOS
	digitalWrite(CMD_BPW34, HIGH);
		
	//DEBUG
	Serial.print("\nbyPyraVolt2 : ");
	Serial.print(byPyraVolt2,DEC);	

}
</code>


ressources utilisées

Source exemple
====== Références ======
Wiki jeenode : [[http://jeelabs.net/projects/hardware/wiki/JeeNode]]

Reference Arduino [[http://arduino.cc/fr/Main/Reference]]

Librairie Arduino : [[http://arduino.cc/fr/Main/Librairies]]
