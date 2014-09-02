#!/bin/sh
# on sauve la liste de paquets installes
cd ~/src/domini/serveur/systeme
dpkg --get-selections > dpkg.txt

echo liste des paquets sauvees
