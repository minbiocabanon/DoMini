#!/bin/sh

#on fixe le baudrate du port ttyUSB0 a 57600 bauds
stty -F /dev/ttyUSB0 57600

#on lance les logiciels
# lancement du daemon qui gère l'onduleur (obligatoire pour remplir la bdd et avoir les notifications pushbullet)
echo "Lancement de ups_daemonized.py"
/usr/bin/python3 ~/src/domini/serveur/ups/ups_daemonized.py &

echo "Lancement de pyreceiver.py"
##la commande suivante permet de donner accés en lecture/écriture à tous les groupes
sudo chmod 666 /dev/ttyUSB0
/usr/bin/python3 ~/src/domini/serveur/pyReceiver/pyreceiver.py &

#on relance le serveur web (plante!?)
#sudo /etc/init.d/lighttpd restart
#sudo /etc/init.d/mysql restart

#lancement service pour ddns
#sudo /usr/local/bin/noip2