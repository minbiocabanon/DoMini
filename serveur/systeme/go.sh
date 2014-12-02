#!/bin/sh

#on fixe le baudrate du port ttyUSB0  à 57600 bauds
stty -F /dev/ttyUSB0 57600
 
#on lance le soft
~/bin/receiver /dev/ttyUSB0&

#on relance le serveur web (plante!?)
#/etc/init.d/lighttpd restart
