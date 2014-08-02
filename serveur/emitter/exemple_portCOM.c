/* Name : Emis_tty1.c Author : Edmond Rohrbacher  Date : 21/2/2007  */
/* Emission d'une chaîne de caractères sur le port série            */
/* Status : opérationnel                                               */
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#define COM0 "/dev/ttyUSB0"
int main (){
	struct termios newconfig;
	char *message = "$BPC,TEST,0\n";
	char cCar_emis;
	int iFdcom2;
	int i, iTaille_msg;
	/* ouverture non bloquante de COM0 */
	if ((iFdcom2 = open (COM0, O_RDWR | O_NONBLOCK | O_NOCTTY)) < 0)
	  perror ("open");
	/* Récupère la config actuelle */
	if (tcgetattr (iFdcom2, &newconfig) < 0)
	perror ("tcgetattr");
	/* Configure la ligne RS 232 COM0 */
	cfsetospeed (&newconfig, B57600); // 19200 Bits/seconde
	newconfig.c_cflag &= ~CSIZE;
	newconfig.c_cflag |= CS8; // 7 bits de Data
	newconfig.c_cflag |= PARENB;
	newconfig.c_cflag |= IGNPAR; // Pas de Parité
	newconfig.c_cflag &= CSTOPB; // 1 stop bit
	/* Modifie la configuration actuelle  */
	if (tcsetattr (iFdcom2, TCSANOW, &newconfig) < 0)
	  perror ("tcsetattr");
	iTaille_msg = strlen (message);
	printf ("Message émis : %s \n", message);
	for (i=0; i< iTaille_msg; i++)
	{     
	 cCar_emis = message[i];
	 printf ("%c ", cCar_emis); 
	 if (write (iFdcom2, &cCar_emis, 1) <0)
		  perror ("write");
	}    
	close (iFdcom2);
	return 0;
}