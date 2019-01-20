#!/bin/sh
#include pour les login et mot de passe du FTP. (ce fichier n'est pas sur le dépot SVN pour la confidentialité)
installpath=/var/www/domini/script/
. $installpath/myvar.sh

# on se  place dans le répertoire de telechargement
cd /tmp
mkdir cache_wget
cd cache_wget

# on commence par créer un fichier contenant son @ IP publique
echo recuperation adresse IP publique
date > myip.txt
curl ipinfo.io/ip >> myip.txt



# on crée le répertoire ou sont stocké les fichers CSV
echo copie des fichiers csv ...
mkdir csv
#on copie les fichiers CSV
cp /var/www/domini/csv/*.csv csv/

#on copie le répertoire avec le style
mkdir css
cp /var/www/domini/css/*.css css/

# on copie toutes les pages du site
echo copie des pages php ...
cp /var/www/domini/*.php .

#on copie les images webcam
# echo copie des webcams ...
# mkdir webcam
# cp -r -n /var/www/domini/webcam/* ./webcam/

#suppression des pages "cas particuliers"
rm -f  /tmp/cache_wget/index.php 
rm -f  /tmp/cache_wget/teleinfo_edf.php
rm -f  /tmp/cache_wget/voletroulant.php
rm -f  /tmp/cache_wget/pellet.php
rm -f  /tmp/cache_wget/chauffage_config.php
rm -f  /tmp/cache_wget/ventilation_flux.php
rm -f  /tmp/cache_wget/map_tondeuse.php
rm -f  /tmp/cache_wget/planning.php
rm -f  /tmp/cache_wget/reseau.php
rm -f  /tmp/cache_wget/restemperature_stat.php

#on charge les pages "cas particuliers
#wget -r http://192.168.0.102:80
curl -O http://0.0.0.0:80/index.php
curl -O http://0.0.0.0:80/teleinfo_edf.php
curl -O http://0.0.0.0:80/voletroulant.php
curl -O http://0.0.0.0:80/pellet.php
curl -O http://0.0.0.0:80/chauffage_config.php
curl -O http://0.0.0.0:80/ventilation_flux.php
curl -O http://0.0.0.0:80/map_tondeuse.php
curl -O http://0.0.0.0:80/planning.php
curl -O http://0.0.0.0:80/temperature_stat.php

#On supprimer l'image du sejour (vie privee!!!)
rm -f  /tmp/cache_wget/webcam/sejour*.jpg
rm -f  /tmp/cache_wget/webcam/garage*.jpg

# upload des fichiers sur le site distant
echo upload du site ...
lftp ftp://$LOGIN:$PASSWORD@ftpperso.free.fr -e "mirror -R /tmp/cache_wget/ /static_domini  ; quit"

#suppression des fichiers temporaires
echo supression des fichiers temporaires ...
rm -f  /tmp/cache_wget/*
rm -f -r /tmp/cache_wget/csv
rm -f -r /tmp/cache_wget/css
rm -f -r /tmp/cache_wget/webcam
