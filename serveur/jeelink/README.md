#ATTENTION !

Le port ttyUSBx doit être utilisabla par l'utilisateur www (www-data la plupart du temps).

(TO DO)
	usermod -a -G dialout www-data (pas suffisant)




## NOTEs

copie des infos provenant d'un port com vers un fichier (fic1)

cat /dev/ttyUSB0 > fic1

copie des infos provenant d'un port com vers un fichier (fic1) et aussi vers le terminal

cat /dev/ttyS1 | tee fic1

changer baudrate

stty -F /dev/ttyUSB0 57600