#!/bin/sh
#include pour les login et mot de passe du FTP. (ce fichier n'est pas sur le dépot SVN pour la confidentialité)
installpath=/var/www/domini/script/
. $installpath/myvar.sh

# on se  place dans le répertoire de telechargement
cd ~/download/
mkdir cache_wget
cd cache_wget

#on ajoute la date courante en début de fichier
date > myip.txt
$'\n' >> myip.txt

# on commence par créer un fichier contenant son @ IP publique
echo recuperation adresse IP publique
curl ipinfo.io/ip >> myip.txt

# upload des fichiers sur le site distant
echo upload du site ...
#lftp ftp://$LOGIN:$PASSWORD@ftpperso.free.fr -e "mirror -R /tmp/cache_wget/ /static_domini  ; quit"
curl -T myip.txt ftp://ftpperso.free.fr/static_domini/ --user $LOGIN:$PASSWORD

#suppression des fichiers temporaires
echo supression des fichiers temporaires ...
rm -f -r ~/download/cache_wget/
# rm -f  ~/download/cache_wget/*
# rm -f -r ~/download/cache_wget/csv
# rm -f -r ~/download/tmp/cache_wget/css
# rm -f -r ~/download/tmp/cache_wget/webcam
