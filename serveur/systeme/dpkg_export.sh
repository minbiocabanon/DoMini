#!/bin/sh
# on sauve la liste de paquets installes
dpkg --get-selections >dpkg.txt

echo liste des paquets sauvees
