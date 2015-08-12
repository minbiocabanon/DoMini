#!/bin/sh

#on fixe le baudrate du port ttyUSB0  à 57600 bauds
stty -F /dev/ttyUSB0 57600

#on lance le soft
#python /home/julien/src/domini/seveur/pyreceiver/pyreceiver
python ~/src/domini/serveur/pyReceiver/pyreceiver.py &
#
#on relance le serveur web (plante!?)
sudo /etc/init.d/lighttpd restart
sudo /etc/init.d/mysql restart
