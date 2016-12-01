// Sketch pour le JeeNode : gestion de la télécommande SOMFY pour les volets roulants
// Réception des ordres via la radio
// Pilote la commande en fonction des ordres
// Date de Création 2011-11-02

#include <JeeLib.h>
#include <math.h>

// PROTOYPES DES FONCTIONS
void clignote_led(void);
int TrtReceptRadio(void);
int check(int consignevr);
void TrtConsigne(char *message);
void change_canal(void);
void set_vr(int consigne);
void status(void);


#define version "JeeNode Gestion Volet Roulant"
#define TIME_0_TO_100	60	//Nb de seconde que mets le registre à parcourir la totalité de sa course
//Declation des variables pour les GPIO
#define MY		17		// Sortie pilotant bouton "My"  - PC3 pin 26 - DIGITAL 17
#define UP		4		//Sortie pilotant bouton "UP"  - PD4 pin 6 - DIGITAL 4
#define DOWN	14		//Sortie pilotant bouton "DOWN"  - PC0 pin 23 - DIGITAL 14
#define CANAL	7		//Sortie pilotant bouton "CANAL"  - PD7 pin 13 - DIGITAL 7

#define DELAY_APPUI_TOUCHE	200	//en milliseconde
#define DELAY_REPOS 5000		// en milliseconde, delai pour attendre que la télécommande passe en mode repos
#define NB_BLIP_LED	500000		//nb de cycle à attendre entr 2 clignotements de la led status

char stNum[] = "VRL";
//chaine pour l'émission RF12
char buffer_recept_rf12[66]="";
long int nblipled = 0;
const int LED=15; 				//Port2 AIO  - DIGITAL 15 


enum VOLET{
	VOLET_FERME, 	// 0
	VOLET_OUVERT,	// 1
	VOLET_MIOMBRE,	// 2
	IMMOBILE		// 3
	};

//----------------------------------------------------------------------
//!\brief           fait clignoter la led une fois
//!\return        -
//----------------------------------------------------------------------
void clignote_led(void){
	int i = 0;

	for(i=0; i<3; i++){
		//allume la led
		digitalWrite(LED, HIGH);
		delay(50);
		//eteint la led
		digitalWrite(LED, LOW);
		delay(100);
	}
}
	
//----------------------------------------------------------------------
//!\brief           Vérifie si le message reçu est un $VRL
//!\return        retourne 1 si le message est $VRL, 0 sinon
//---------------------------------------------------------------------- 
int TrtReceptRadio(void){
	// si on a reçu un message valide
	if (rf12_recvDone() && rf12_crc == 0){
		// DEBUG
		//Serial.print("message recu");
		
		//on copie le buffer reçu dans un buffer de travail
		memcpy(buffer_recept_rf12, (void*) rf12_data, rf12_len);
		// terminateur de chaine
		// à tester ???
		buffer_recept_rf12[rf12_len]=0;
		
		//on verifie que l'entet du message est la bonne, en théorie c'est toujours le cas si on utilise les ID radio
		if( buffer_recept_rf12[0] == '$' && buffer_recept_rf12[1] == 'V' && buffer_recept_rf12[2] == 'R' && buffer_recept_rf12[3] == 'L'){	
			//on affiche le message reçu
			Serial.println(buffer_recept_rf12);
			//si un ACK de réception du message est demandé, on envoi un buffer vide avec ACK dans l'entete
			if(RF12_WANTS_ACK){
				rf12_sendStart(RF12_ACK_REPLY, 0, 0);
				Serial.println("\n ACK sent");
			}
			// on retourne 1
			return(1);
		}
		else{
			// on écrase le buffer (par sécurité)
			memcpy(buffer_recept_rf12, 0, rf12_len);
			// on retourne 0
			return(0);
		}

	}
	else{
		// on écrase le buffer (par sécurité)
		memcpy(buffer_recept_rf12, 0, rf12_len);
		return(0);
	}
}

//----------------------------------------------------------------------
//!\brief        Verifie que la consigne recue a bien une valeur cohérente
//!\param        consigne volet
//!\return		1 si consigne est valide, 0 sinon
//---------------------------------------------------------------------- 
int check(int consignevr){
	//on compare aux différentes valeur possible
	if(consignevr == VOLET_FERME || consignevr == VOLET_OUVERT || consignevr == VOLET_MIOMBRE || consignevr == IMMOBILE ){
		// si la consigne existe on retourne 1
		return(1);
	}
	else{
		return(0);
	}
}

//----------------------------------------------------------------------
//!\brief           Traite la consigne reçue
//!\param        message reçu
//---------------------------------------------------------------------- 
void TrtConsigne(char *message){

	//Déclaration des variables
	char *entete, *i;
	int consigne_bureau, consigne_salon, consigne_chm, consigne_chjf;

	Serial.print("\nOn analyse le message recu");
	
	//on depouille le message reçu
	entete = strtok_r(message, ",", &i);
	consigne_bureau = atoi(strtok_r(NULL, ",", &i)); 
	consigne_salon = atoi(strtok_r(NULL, ",", &i)); 
	consigne_chm = atoi(strtok_r(NULL, ",", &i)); 
	consigne_chjf = atoi(strtok_r(NULL, ",", &i)); 
	
	// on verifie que les valeurs recuperees sont coherentes avant de les envoyer (pour eviter cas de messages corrompus recus)
	if( check(consigne_bureau) == 1 && check(consigne_salon) == 1 && check(consigne_chm) == 1 && check(consigne_chjf) == 1 ){
		// les consignes sont ok pour appliques les consignes aux volets :
		Serial.print("\nConsignes verifiees, on peut les appliquer.");
		Serial.print("\nPilotage de la telecommande");
		//on sélectionne le premier canal, le premier appui active le canal 1
		Serial.print("\nSelection canal 1");
		//on appui 2 fois car par défaut, le canal 5 est sélectionné
		change_canal();
		change_canal();
		// on envoie la consigne pour le bureau
		set_vr(consigne_chjf);
		
		// apres une consigne, il faut appuyer 2 fois sur le bouton canal pour changer de canal
		Serial.print("\nSelection canal 2");
		change_canal();
		// on envoie la consigne pour le salon
		set_vr(consigne_chm);
		
		// apres une consigne, il faut appuyer 2 fois sur le bouton canal pour changer de canal
		Serial.print("\nSelection canal 3");
		change_canal();
		// on envoie la consigne pour la chm
		set_vr(consigne_salon);
		
		// apres une consigne, il faut appuyer 2 fois sur le bouton canal pour changer de canal
		Serial.print("\nSelection canal 4");
		change_canal();
		// on envoie la consigne pour la chjf
		set_vr(consigne_bureau);

		// on saute le canal 5 et on revient au canal1 : 2 appui pour passer au canal 5 + 1 appui pour passer au canal1
		Serial.print("\nSelection canal 5");
		change_canal();
		
		Serial.print("\nSequence terminee");
		
		//on attend 5000ms pour attendre que la téléco passe au repos (sinon risque de réglage si on reçoit deux commandes successives par la radio)
		delay(DELAY_REPOS);
	
	}
	// si c'est pas bon, il ne faut pas transmettre la consigne. 
	else{
		// on affiche du debug
		Serial.println("\nConsigne erronnee, on ne fait rien.\nConsigne_bureau:");
		Serial.println(consigne_bureau);
		Serial.println("consigne_salon:");
		Serial.println(consigne_salon);
		Serial.println("consigne_chm:");
		Serial.println(consigne_chm);
		Serial.println("consigne_chjf:");
		Serial.println(consigne_chjf);
	}
}

//----------------------------------------------------------------------
//!\brief      Réalise un changement de canal sur la télécommande
//!\brief	Appui de 0.5s sur le bouton canal
//---------------------------------------------------------------------- 
void change_canal(void){
	//on appui sur le bouton canal
	digitalWrite(CANAL, 1);
	//on attend
	delay(DELAY_APPUI_TOUCHE);
	// on relache le bouton canal
	digitalWrite(CANAL, 0);
	//on attend 200ms
	delay(DELAY_APPUI_TOUCHE);

}

//----------------------------------------------------------------------
//!\brief      Réalise un appui sur la touche de canal sur la télécommande
//!\param        touche : bouton sur lequel il faut réaliser l'appui
//---------------------------------------------------------------------- 
void appui_touche(int touche){
			// appui sur la touche 
			digitalWrite(touche, 1);
			//on attend 
			delay(DELAY_APPUI_TOUCHE);
			// on relache la touche 
			digitalWrite(touche, 0);
			//on attend 
			delay(DELAY_APPUI_TOUCHE);
}

//----------------------------------------------------------------------
//!\brief		Faire monter/descendre le volet 
//!\param		entrée : consigne Up/ down / My
//---------------------------------------------------------------------- 
void set_vr(int consigne){
	
	Serial.print("\nEnvoi consigne");
	switch(consigne){
		// si la consigne est de FERMER le volet
		case VOLET_FERME:
			appui_touche(DOWN);
			//on appui a nouveau sur canal pour faire clignoter le canal en cours (le prochain appui fera changer de canal)
			change_canal();			
			break;
		// si la consigne est d'OUVRIR le volet
		case VOLET_OUVERT:
			appui_touche(UP);
			//on appui a nouveau sur canal pour faire clignoter le canal en cours (le prochain appui fera changer de canal)
			change_canal();
			break;
		// si la consigne est de FERMER le volet
		case VOLET_MIOMBRE:
			appui_touche(MY);
			//on appui a nouveau sur canal pour faire clignoter le canal en cours (le prochain appui fera changer de canal)
			change_canal();
			break;	
		// si la consigne est de ne pas bouger le volet
		case IMMOBILE:
			// on ne fait rien! 
			delay(DELAY_APPUI_TOUCHE);
			break;
	}
	
	//on attend un peu pour la prochaine consigne
	delay(DELAY_APPUI_TOUCHE);

}

//----------------------------------------------------------------------
//!\brief           positionne le canal de la télécommande
//!\return        -
//----------------------------------------------------------------------
void select_canal(int num_canal){
	
	//on active la sélection de canal
	change_canal();
	
	//tant qu'on est pas sur canal passé en paramètre, on change de canal
	for( int i=1; i < num_canal; i++)
		change_canal();
		
	//on attend 5000ms pour attendre que la téléco passe au repos (sinon risque de réglage si on reçoit deux commandes successives par la radio)
	delay(DELAY_REPOS);
}

//----------------------------------------------------------------------
//!\brief           fait clignoter une led en fond de tache pour donner un signe de vie
//!\return        -
//----------------------------------------------------------------------
void status(void){
	
	nblipled++;
	if(nblipled > NB_BLIP_LED){
		//allume la led
		digitalWrite(LED, LOW);
		delay(50);
		//eteint la led
		digitalWrite(LED, HIGH);
		nblipled=0;		
	}
}

//----------------------------------------------------------------------
//!\brief		Setup
//---------------------------------------------------------------------- 
void setup() {
	// init port COM du debug
	Serial.begin(57600);
	// affichage de la version
	Serial.println(version);
	Serial.println();
	Serial.println("Num. de Jeenode : ");
	Serial.println(stNum);
	
	//config des GPIO
	pinMode(MY, OUTPUT);
	pinMode(UP, OUTPUT);
	pinMode(DOWN, OUTPUT);
	pinMode(CANAL, OUTPUT);
	
	// ------- Broches en sortie -------  
	pinMode(LED, OUTPUT); //met la broche en sortie 
	digitalWrite(LED, LOW);
	
	clignote_led();
	
	//initialisation des GPIO	
	digitalWrite(MY, 0);
	digitalWrite(UP, 0);
	digitalWrite(DOWN, 0);
	digitalWrite(CANAL, 0);
	
	//on positionne la télécommande sur un canal inutilisé : permet de résoudre les pbs de volets qui s'ouvrent/se ferment tout seul en plein nuit sur parasitage....
	//select_canal(5);

	//initialisation du module radio RF12
	rf12_initialize(1, RF12_868MHZ, 33);
	// Reconfig du baudrate : 1200 bauds
	rf12_control(0xC6A3);	
	Serial.print("\nInit Radio : \n 1200 bauds\n");
}

//----------------------------------------------------------------------
//!\brief		Boucle principale
//---------------------------------------------------------------------- 
void loop() {
	
	//si le message radio est correcte
	if (TrtReceptRadio() == 1 ){
		//on fait clignoter la led pour indiquer qu'on a reçu un message valide
		clignote_led();
		
		//on execute la consigne du message recu
		TrtConsigne(buffer_recept_rf12);
	}	
	status();
}
