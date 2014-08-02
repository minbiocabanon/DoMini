// Sketch pour le Jeenlink qui se comporte en mode transparent
// Les messages série doivent commencer par $ et se terminer par LF pour etre transmis par radio
// Date de Création 2010-11-25

#include <Ports.h>
#include <RF12.h>
#include <RF12sio.h>

#define version "JeeLink Serial to RF12 transparent mode RzBo-Jenipapo V1.0 25/11/2010"
#define LED_PIN 9

// Etats de la SM de reception serie
// Etat initial sans objectif 
#define SM_SERIAL_NIL 0
// attente du debut de message
#define SM_SERIAL_WAIT_DEB 1
// traitement du corps du message
#define SM_SERIAL_MSG 2

unsigned int serial_state = SM_SERIAL_NIL;

String str_payload = "";
boolean bMsg_serie_recu = false;



void setup() {
	Serial.begin(57600);
	Serial.println(version);
	
	
	//initialisation de la sortie qui pilote la led
	pinMode(LED_PIN, OUTPUT);
	//on eteind la led
	digitalWrite(LED_PIN, 1);	
	
	//initialisation du module radio RF12
	rf12_initialize(1, RF12_868MHZ, 33);
	// Reconfig du baudrate : 1200 bauds
	rf12_control(0xC6A3);
	Serial.print("\nInit Radio : \n 1200 bauds");
	
	// initialisation de la SM série
	serial_state = SM_SERIAL_WAIT_DEB;
}

void loop() {

	if (Serial.available() > 0){
		char charin = Serial.read();
		
		// en fonction de l'etat de la state machine on avise
		switch(serial_state) {
			// on est dans le neant
			case SM_SERIAL_NIL:
				// on ne fait rien
				break;
			case SM_SERIAL_WAIT_DEB:
				// on a trouve det du msg = "$"
				if(charin == '$') {
					// on prepare le buffer
					str_payload = ""; 
					// on sauve ce char
					str_payload += charin;
					// on change d'etat pour passer au corps du message
					serial_state = SM_SERIAL_MSG;
					// On trace pour debug
					// Serial.print("debut detecte");
					// Serial.print(charin,HEX);
					// Serial.print(" - (ascii) :");
					// Serial.print(charin,BYTE);
					// Serial.print("\r\n");
				} else{
					// Serial.print(" caractere jete (hex):");
					// Serial.print(charin,HEX);
					// Serial.print(" - (ascii) :");
					// Serial.print(charin,BYTE);
					// Serial.print("\r\n");
				}
				break;
			case SM_SERIAL_MSG:
				// on sauve ce char
				str_payload += charin;
				// on a atteint la fin du msg = CR
				if(charin == 0x0D) {
					// on change d'etat, message suivant
					// discutable, a preciser depend du traitement du message
					serial_state = SM_SERIAL_WAIT_DEB;
					// on leve un flag pour indiquer que le msg est complet
					bMsg_serie_recu = true;
					// trace pour debug
					// Serial.print("fin detectee \n\r");
					// Serial.print(charin,HEX);
					// Serial.print(" - (ascii) :");
					// Serial.print(charin,BYTE);
					// Serial.print("\r\n");
				}
				break;
		}
	}
		
		
	if (rf12_canSend() && bMsg_serie_recu == true){
		// taille limitee à 64 octets par la lib RF12
		char payload[64];
		str_payload.toCharArray(payload, sizeof(payload));
		int nPayloadSize = str_payload.length();
		// DEBUG
		// Serial.print("\r\n Msg a emettre : ");
		// for(byte i=0; i < nPayloadSize; i++)
			// Serial.print(payload[i]);
			
		// envoi du message vers la radio
		rf12_sendStart(0, payload, nPayloadSize);
		
		// on reset le flag
		bMsg_serie_recu = false;
	}
	

	if (rf12_recvDone() && rf12_crc == 0){
		//on allume la led
		digitalWrite(LED_PIN, 0);
		
		// DEBUG
		//Serial.print("message recu");
		for (byte i = 0; i < rf12_len; ++i)
			Serial.print(rf12_data[i]);

		//on eteind la led
		digitalWrite(LED_PIN, 1);
	}
			
		
		delay(10);
}
