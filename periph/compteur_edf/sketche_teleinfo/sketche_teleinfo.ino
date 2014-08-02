// Sketch pour envoi de données téléinfo compteur EDF via radio
// Messages téléinfo reu via le portCOM
// Date de Création 2010-11-11
// Calcul du CRC par Boris H.

#include <JeeLib.h>
#include <avr/wdt.h>
//#include <PString.h>

#define version "JeeNode TeleInfo"
#define DEBUG 1
#define LED_PIN 9

// états de la SM de réception série
// état initial sans objectif 
#define SM_SERIAL_NIL 0
// attente du début de message
#define SM_SERIAL_WAIT_DEB 1
// traitement du corps du message
#define SM_SERIAL_MSG 2

#define NBMSG 10
int nNbmsg = 0; 

MilliTimer sendTimer;
char payload[] = "BUFFER VIDE";
char buffer_recept_teleinfo[180];

String str_buffer_recept_teleinfo = "";
String str_buffer_data = "";
String str_EnteteMsg = "";
String str_DonneeMsg = "";
String str_ADCO;
String str_OPTARIF;
String str_ISOUSC;
String str_HCHC;
String str_HCHP;	
String str_PTEC;			
String str_IINST;
String str_IMAX;
String str_PAPP;
String str_HHPHC;
String str_MOTDETAT;

boolean bMsg_teleinfo_recu = false;
boolean bTraite_msg = false;
boolean bEnvoi_radio = false;
boolean bFlag_ADCO_recu = false;
boolean bFlag_OPTARIF_recu = false;
boolean bFlag_ISOUSC_recu = false;
boolean bFlag_HCHC_recu = false;
boolean bFlag_HCHP_recu = false;
boolean bFlag_PTEC_recu = false;	
boolean bFlag_IINST_recu = false;
boolean bFlag_IMAX_recu = false;
boolean bFlag_PAPP_recu = false;
boolean bFlag_HHPHC_recu = false;
boolean bFlag_MOTDETAT_recu = false;

int i = 0;
unsigned int nlength = 180;
unsigned int serial_state = SM_SERIAL_NIL;

//----------------------------------------------------------------------
//!\brief           Vérifie si le CRC du message issu du compteur est valide
//!\brief          Le checksum est calculé sur l'ensemble des caractères allant du début du champ étiquette à la fin du champ 
//!\brief          donnée, caractère SP inclus. On fait tout d'abord la somme des codes ASCII de tous ces caractères. Pour éviter
//!\brief          d'introduire des fonctions ASCII (0x00  à 0x1F), on ne conserve  que les six bits de poids faible du 
//!\brief          résultat obtenu (cette opération se traduit par un ET logique entre la somme précédemment calculée et 03Fh). 
//!\brief          Enfin, on ajoute 0x20h. Le résultat sera donc toujours un caractère ASCII imprimable (signe, chiffre, 
//!\brief          lettre majuscule) allant de 0x20 à 0x5F */
//!\param        buffer
//!\return        retourne TRUE si le CRC est valid
//---------------------------------------------------------------------- 
boolean validCRC(String teleinfo_line) {

  char CRC;
  char runningSum;
  int  x;
  
  CRC = teleinfo_line.charAt(teleinfo_line.length() - 2);
  x = runningSum = 0;
  while( x < (teleinfo_line.length() - 2)){
    runningSum += teleinfo_line.charAt(x);
    x++;
  }	
  runningSum = runningSum & 0x03F;
  if (runningSum < 0x20) runningSum += 0x40; // je ne sais pas pourquoi, mais ça marche
  return (runningSum == CRC)?true:false;
}

//----------------------------------------------------------------------
//!\brief           Vérifie si le chaine transmise contient un nombre
//!\param        chaine à traiter
//!\return        retourne TRUE si la chaine contient bien un nombre
//---------------------------------------------------------------------- 
boolean validNumber(String str) {
  //Check if each character is a 0-9 digit
  for (int i=0; i<str.length(); i++) {
    if ((str.charAt(i) < 0x30) || (str.charAt(i) > 0x39)) 
       return false;
  }
  return true;  
}

//----------------------------------------------------------------------
//!\brief		Setup
//---------------------------------------------------------------------- 
void setup () {
	// initialisation du port 0-1 : 1200,7,E,1 conection téléinfo
	Serial.begin(1200);
	UCSR0C = B00100100;

	// initialisation du port COM
	//Serial.begin(57600);
	//Serial.println(57600);
	Serial.println(version);

	//initialisation du module radio RF12
	rf12_initialize(1, RF12_868MHZ, 33);
	// Reconfig du baudrate : 1200 bauds
	rf12_control(0xC6A3);
	Serial.print("\nInit Radio : \n 1200 bauds");

	// initialisation de la SM série
	serial_state = SM_SERIAL_WAIT_DEB;
	
	//initialisation de la sortie qui pilote la led
	pinMode(LED_PIN, OUTPUT);	
	//on éteind la led
	digitalWrite(LED_PIN, 1);
	
	  wdt_enable (WDTO_2S);  // reset after one second, if no "pat the dog" received
}

//----------------------------------------------------------------------
//!\brief		Boucle principale
//---------------------------------------------------------------------- 
void loop () {
	
	char charin = 0;

	// **************************************************
	// *	Recuperation des donnees de teleinfo
	// **************************************************
	// si on recoit un caractere
	if(Serial.available() > 0) {
		// on lit le caractère disponible
		charin = Serial.read();
		// Serial.print(" caractere lu (hex):");
		// Serial.print(charin,HEX);
		// Serial.print(" - (ascii) :");
		// Serial.print(charin,BYTE);
		// Serial.print("\r\n");
		
		// en fonction de l'état de la state machine on avise
		switch(serial_state) {
			// on est dans le néant
			case SM_SERIAL_NIL:
				// on ne fait rien
				break;
			case SM_SERIAL_WAIT_DEB:
				// on a trouvé le début du msg = LF
				if(charin == 0x0A) {
					// on prépare le buffer
					str_buffer_recept_teleinfo = "";  
					// on change d'état pour passer au corps du message
					serial_state = SM_SERIAL_MSG;
					//On trace pour debug
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
				str_buffer_recept_teleinfo += charin;
				// on a atteint la fin du msg = CR
				if(charin == 0x0D) {
					// on change d'état, message suivant
					// discutable, à préciser dépend du traitement du message
					serial_state = SM_SERIAL_WAIT_DEB;
					// on lève un flag pour indiquer que le msg est complet
					bMsg_teleinfo_recu = true;
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
	
	
	// **************************************************
	// *	Depouillement du message reçu 
	//**************************************************
	if(bMsg_teleinfo_recu == true){
		// on récupère la position du premier espace
		//int nIndiceDonnee = str_buffer_recept_teleinfo.indexOf(" ");
		i = 0;
		
		//on allume la led
		digitalWrite(LED_PIN, 0);
		
		// tant qu'on ne lit pas un caractère espace
		while(str_buffer_recept_teleinfo.charAt(i) != 0x20){
			str_EnteteMsg += str_buffer_recept_teleinfo.charAt(i);
			i++;
		}
		
		// DEBUG
		// Serial.println("\n\r entete :" + str_EnteteMsg + "-");
						
		//on saute le caractère espace
		i++;
		// tant qu'on ne lit pas un caractère espace
		while(str_buffer_recept_teleinfo.charAt(i) != 0x20){
			str_DonneeMsg += str_buffer_recept_teleinfo.charAt(i);
			i++;
		}
				
		// DEBUG
		// Serial.println("\n\r donnee :" + str_DonneeMsg + "-");
         // on vérifie le CRC
		if (validCRC(str_buffer_recept_teleinfo) == false ) {
		  str_EnteteMsg = "";
		}

		
		if(str_EnteteMsg == "ADCO"){
			// Serial.println("\n\r ADCO " + str_EnteteMsg +" = " + str_DonneeMsg + "\n\r");
			str_ADCO = str_DonneeMsg;
			bFlag_ADCO_recu = true;
		}else if(str_EnteteMsg == "OPTARIF"){
			// Serial.println("\n\r OPTARIF " + str_EnteteMsg +" = " + str_DonneeMsg + "\n\r");
			str_OPTARIF = str_DonneeMsg;
			bFlag_OPTARIF_recu = true;
		}else if(str_EnteteMsg == "ISOUSC"){
			// Serial.println("\n\r ISOUSC " + str_EnteteMsg +" = " + str_DonneeMsg + "\n\r");
			str_ISOUSC = str_DonneeMsg;
			bFlag_ISOUSC_recu = true;
		}else if(str_EnteteMsg == "HCHC" && validNumber(str_DonneeMsg) && str_DonneeMsg.length() == 9){
			//conso heure creuse 8chars
			// Serial.println("\n\r HCHC " + str_EnteteMsg +" = " + str_DonneeMsg + "\n\r");
			str_HCHC = str_DonneeMsg;
			bFlag_HCHC_recu = true;
		}else if(str_EnteteMsg == "HCHP" && validNumber(str_DonneeMsg) && str_DonneeMsg.length() == 9){
			// conso heure pleine 8chars		
			// Serial.println("\n\r HCHP " + str_EnteteMsg +" = " + str_DonneeMsg + "\n\r");
			str_HCHP = str_DonneeMsg;
			bFlag_HCHP_recu = true;
		}else if(str_EnteteMsg == "PTEC"){
			// Serial.println("\n\r PTEC " + str_EnteteMsg +" = " + str_DonneeMsg + "\n\r");
			str_PTEC = str_DonneeMsg;
			bFlag_PTEC_recu = true;
		}else if(str_EnteteMsg == "IINST"){
			// Serial.println("\n\r IINST " + str_EnteteMsg +" = " + str_DonneeMsg + "\n\r");
			str_IINST = str_DonneeMsg;
			bFlag_IINST_recu = true;
		}else if(str_EnteteMsg == "IMAX"){
			// intensité max 3 chars
			// Serial.println("\n\r IMAX " + str_EnteteMsg +" = " + str_DonneeMsg + "\n\r");
			str_IMAX = str_DonneeMsg;
			bFlag_IMAX_recu = true;
		}else if(str_EnteteMsg == "PAPP"){
			// Serial.println("\n\r PAPP " + str_EnteteMsg +" = " + str_DonneeMsg + "\n\r");
			str_PAPP= str_DonneeMsg;
			bFlag_PAPP_recu = true;
		}else if(str_EnteteMsg == "HHPHC"){
			// Serial.println("\n\r HHPHC " + str_EnteteMsg +" = " + str_DonneeMsg + "\n\r");
			str_HHPHC = str_DonneeMsg;
			bFlag_HHPHC_recu = true;
		}else if(str_EnteteMsg == "MOTDETAT"){
			// Serial.println("\n\r MOTDETAT " + str_EnteteMsg +" = " + str_DonneeMsg + "\n\r");
			str_MOTDETAT = str_DonneeMsg;
			bFlag_MOTDETAT_recu = true;
		}
				
		// Tri des messages : si les messages intéressants sont reçu, on autorise l'envoi radio
		// le flag bFlag_MOTDETAT_recu est utilisé pour déclencher l'envoi afin d'attendre la réception de la totalité du message téléinfo avant l'envoi sur la radio
		bEnvoi_radio = bFlag_HCHC_recu & bFlag_HCHP_recu & bFlag_PTEC_recu & bFlag_IINST_recu & bFlag_IMAX_recu & bFlag_PAPP_recu & bFlag_MOTDETAT_recu;
		
		// on incrémente le nombre de message complet recu
		if(bEnvoi_radio == true)
			nNbmsg++;
		
		//Pour limiter la quantité de message, on envoi un message tous les nNbmsg
		if((bEnvoi_radio == true) && (nNbmsg <= NBMSG)){
			bEnvoi_radio = false;
			bFlag_HCHC_recu = bFlag_HCHP_recu = bFlag_PTEC_recu = bFlag_IINST_recu = bFlag_IMAX_recu = bFlag_PAPP_recu =false ;
		}
		
		// on reset le flag de traitement du message
		bMsg_teleinfo_recu = false;

		// on vide les buffer pour la prochaine trame
		str_EnteteMsg = "";
		str_DonneeMsg = "";
		//on vide le buffer de travail
		str_buffer_recept_teleinfo = "";

		//on éteind la led
		digitalWrite(LED_PIN, 1);
	}

	// **************************************************
	// *	Emission des infos TéléInfo sur la radio
	// **************************************************
	
	if(bEnvoi_radio == true){
		//on allume la led
		digitalWrite(LED_PIN, 0);

		//Préparation du buffer à emettre par radio
		String str_buffer_radio = "$EDF," + str_HCHC + "," + str_HCHP + "," + str_PTEC + "," + str_IINST + "," + str_IMAX + "," + str_PAPP + "\r\n";
		
		// DEBUG
		//Serial.println("\n\r buffer radio :" + str_buffer_radio + "-");

		// conversion du string en buffer pour la radio
		char payload[]="$EDF,000000000,000000000,XX..,000,000,00000**";
		//DEBUG
		Serial.println("\n\rBuffer radio a emettre: ");
		for(byte i=0; i < sizeof payload; i++){
			payload[i] = str_buffer_radio.charAt(i);
			// DEBUG
			// Serial.print(payload[i]);
		}


		//tant que porteuse pas libre
		while (!rf12_canSend())
			//on interroge le module
			rf12_recvDone();
		// porteuse libre, on envoie le message
		rf12_sendStart(0, payload, sizeof payload);
		// Serial.print("\n\rMessage envoye OK\n\r");
		
		// raz des flags
		bFlag_HCHC_recu = bFlag_HCHP_recu = bFlag_PTEC_recu = bFlag_IINST_recu = bFlag_IMAX_recu = bFlag_PAPP_recu =false ;
		bEnvoi_radio = false;
		nNbmsg = 0;

		//on éteind la led
		digitalWrite(LED_PIN, 1);
	}
	
	  wdt_reset ();  // give me another second to do stuff (pat the dog)
}

