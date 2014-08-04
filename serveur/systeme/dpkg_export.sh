#!/bin/sh
# on se  place dans le répertoire ou sauver les donnees
cd ~/serveur/systeme/

# on sauve la liste de paquets installes
dpkg --get-selections >dpkg.txt

echo liste des paquets sauvees
