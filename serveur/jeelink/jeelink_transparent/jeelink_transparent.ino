// Sketch pour le Jeenlink qui se comporte en mode transparent
// Les messages série doivent commencer par $ et se terminer par LF pour etre transmis par radio

#include <JeeLib.h>

#define version "JeeLink Serial to RF12 transparent mode 2013-02-21"
#define LED_PIN 9

// Mode radio
// Values 2 and 3 can cause the millisecond time to lose a few interrupts. 
// Value 3 can only be used if the ATmega fuses have been set for fast startup, i.e. 258 CK - the default Arduino fuse settings are not suitable for full power down.
#define MODE_RADIO_NORMAL 0
#define MODE_RADIO_IDLE 1
#define MODE_RADIO_STANDBY 2
#define MODE_RADIO_PWR_DOWN 3


// Etats de la SM de reception serie
// Etat initial sans objectif 
#define SM_SERIAL_NIL 0
// attente du debut de message
#define SM_SERIAL_WAIT_DEB 1
// traitement du corps du message
#define SM_SERIAL_MSG 2

#define ACK_TIME 5000
#define NB_ATTEMPTS_ACK 3

unsigned int serial_state = SM_SERIAL_NIL;

String str_payload = "";
boolean bMsg_serie_recu = false;
boolean bAck_dmd = false;

char buffer_recept_rf12[66]="";

//----------------------------------------------------------------------
//!\brief		Gère l'attente pour la réception d'un message vide avec ACK
//!\return        -
//----------------------------------------------------------------------
static byte waitForAck(void){
	MilliTimer ackTimer;
	while (!ackTimer.poll(ACK_TIME)){
		if (rf12_recvDone() && rf12_crc == 0 && ((rf12_hdr & RF12_HDR_ACK) == 0) && ((rf12_hdr & RF12_HDR_CTL) == 128) ){
			// Serial.println("ACK Received");
			// Serial.print("Node ID:");Serial.println(rf12_hdr & RF12_HDR_MASK); 
			// Serial.println("received something");
			// Serial.println(rf12_hdr);
			// Serial.print("RF12_HDR_DST=");Serial.println(rf12_hdr & RF12_HDR_DST);
			// Serial.print("RF12_HDR_CTL=");Serial.println(rf12_hdr & RF12_HDR_CTL);
			// Serial.print("RF12_HDR_ACK=");Serial.println(rf12_hdr & RF12_HDR_ACK);
			// Serial.print("rf12_len=");Serial.println(rf12_len);
			return 1;
		}
	}
	return 0;
}

//----------------------------------------------------------------------
//!\brief		Gère la réception des chaines de caractères sur le port Serial
//!\return        -
//----------------------------------------------------------------------
void trt_serial(void){
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
				if(charin == '$' || charin == '!') {
					// on prepare le buffer
					str_payload = ""; 
					// on sauve ce char
					str_payload += charin;
					// on change d'etat pour passer au corps du message
					serial_state = SM_SERIAL_MSG;
					//on allume la led
					digitalWrite(LED_PIN, 0);
					// On trace pour debug
					// Serial.print("debut detecte");
					// Serial.print(charin,HEX);
					// Serial.print(" - (ascii) :");
					// Serial.print(charin,BYTE);
					// Serial.print("\r\n");
					//si le message commence par un !, on veut avoir un ACK radio
					if(charin == '!')
						bAck_dmd = true;
						
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
				// on a atteint la fin du msg = LF (ou \n)
				if(charin == 0X0A || charin == 0X0D) {
					// on change d'etat, message suivant
					serial_state = SM_SERIAL_WAIT_DEB;
					// on leve un flag pour indiquer que le msg est complet
					bMsg_serie_recu = true;
					//on allume la led
					digitalWrite(LED_PIN, 1);
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
}

//----------------------------------------------------------------------
//!\brief		Gère l'envoi des messages sur la radio 
//!\brief		Traite également de la nécessite d'envoyer un message avec demande d'ACK , bAck_dmd = true
//!\return        -
//----------------------------------------------------------------------
void trt_message_to_send(void){
	int nAttempt = 1;
	bool flag_ACK_received = false;
	
	if (bMsg_serie_recu == true){
		// taille limitee à 64 octets par la lib RF12
		char payload[64];
		str_payload.toCharArray(payload, sizeof(payload));
		int nPayloadSize = str_payload.length();
		// DEBUG
		// Serial.print("\n Msg a emettre \n");
		// for(byte i=0; i < nPayloadSize; i++)
			// Serial.print(payload[i]);
		// Serial.println();
		//tant que porteuse pas libre
		
		//si le message ne demande pas de gestion d'ACK
		if(bAck_dmd == false){
			while (!rf12_canSend())
			//on interroge le module
				rf12_recvDone();
			
			// envoi du message vers la radio
			//rf12_sendStart(0, payload, sizeof (payload));
			rf12_sendStart(0, payload, nPayloadSize);
			
			// DEBUG
			// Serial.print("\r\n Msg emis");
			
			//attente de la fin d'émission radio
			rf12_sendWait(MODE_RADIO_IDLE);		
		}
		//sinon le message à envoyer nécessite un traitement d'ACK
		else{
			// tant que le nb de tentative n'est pas dépassé ou qu'on a pas reçu un ACK
			while(nAttempt < NB_ATTEMPTS_ACK && !flag_ACK_received ){
				// on attend que le module soit dispo pour l'emission
				while (!rf12_canSend())
					rf12_recvDone();
				// on envoie le message avec le bit ACK
				rf12_sendStart(RF12_HDR_ACK, payload, sizeof payload);
				rf12_sendWait(1);
				//DEBUG
				// Serial.print("Attemps : ");Serial.println(nAttempt);
				
				if (waitForAck()){
					// Serial.println("ACK received\n");
					flag_ACK_received = true;
				}else 
					// Serial.println("ACK NOK received\n");
				// on incrémente le compteur de tentative d'ACK
				nAttempt++;
			}
			// on reset le flag pour le prochain coup
			bAck_dmd = false;
		}
		
		// on reset le flag
		bMsg_serie_recu = false;
	}
	delay(20);
}

//----------------------------------------------------------------------
//!\brief		Gère la réception des messages radio avec gestion des ACK
//!\return        -
//----------------------------------------------------------------------
void trt_radio_recept(void){
	
	if (rf12_recvDone() && rf12_crc == 0){
		//on allume la led
		digitalWrite(LED_PIN, 0);
		
		//on copie le buffer reçu dans un buffer de travail
		memcpy(buffer_recept_rf12, (void*) rf12_data, rf12_len);
		// terminateur de chaine
		// à tester ???
		buffer_recept_rf12[rf12_len]=0;
		
		//si le message reçu demande un ACK
		if(RF12_WANTS_ACK){
			//Alors on renvoi un message vide avec le header ACK
			rf12_sendStart(RF12_ACK_REPLY, 0, 0);
			//Serial.println("\n ACK envoyé");
		}	

		Serial.print(buffer_recept_rf12);
		//on eteind la led
		digitalWrite(LED_PIN, 1);
	}		
	delay(20);

}

//----------------------------------------------------------------------
//!\brief		Setup
//!\return        -
//----------------------------------------------------------------------
void setup(void) {
	Serial.begin(57600);
	Serial.println(version);
		
	//initialisation de la sortie qui pilote la led
	pinMode(LED_PIN, OUTPUT);
	//on eteind la led
	digitalWrite(LED_PIN, 1);	
	
	//initialisation du module radio RF12
	rf12_initialize(31, RF12_868MHZ, 33);
	// Reconfig du baudrate : 1200 bauds
	rf12_control(0xC6A3);
	Serial.print("\nInit Radio : \n 1200 bauds\n");
	
	// initialisation de la SM série
	serial_state = SM_SERIAL_WAIT_DEB;
}

//----------------------------------------------------------------------
//!\brief		Boucle principale
//!\return        -
//----------------------------------------------------------------------
void loop(void) {

	trt_serial();
		
	trt_message_to_send();

	trt_radio_recept();
}