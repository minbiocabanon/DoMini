#!/bin/sh
# on se  place dans le répertoire ou sauver les donnees
cd /media/dd_usb/SHEEVA_SERVER/systeme/

# on sauve la liste de paquets installes
dpkg --get-selections >dpkg.txt

echo liste des paquets sauvees
