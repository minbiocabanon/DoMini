# jeelink_transparent_noack
Ce sketche est pour le test uniquement 
	si plusieurs jeelink sont présents en même temps de le réseau, il ne faut qu'un seul Jeelink qui retourne les ACK (sinon c'est le plus rapide qui répond et le message peut etre perdu pour le serveur principal)


#ATTENTION !

Le port ttyUSBx doit être utilisabla par l'utilisateur www (www-data la plupart du temps).

	usermod -a -G dialout www-data
	ou
	adduser www-data dialout




## NOTEs

copie des infos provenant d'un port com vers un fichier (fic1)

cat /dev/ttyUSB0 > fic1

copie des infos provenant d'un port com vers un fichier (fic1) et aussi vers le terminal

cat /dev/ttyS1 | tee fic1

changer baudrate

stty -F /dev/ttyUSB0 57600
