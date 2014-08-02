#!/bin/sh
#include pour les login et mot de passe du FTP. (ce fichier n'est pas sur le dépot SVN pour la confidentialité)
installpath=/media/dd_usb/SHEEVA_SERVER/www/domini/script/
. $installpath/myvar.sh

# on se  place dans le répertoire de telechargement
cd /media/dd_usb/cache_wget/192.168.0.102/

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
rm /media/dd_usb/cache_wget/192.168.0.102/index.php 
rm /media/dd_usb/cache_wget/192.168.0.102/teleinfo_edf.php
rm /media/dd_usb/cache_wget/192.168.0.102/voletroulant.php
rm /media/dd_usb/cache_wget/192.168.0.102/pellet.php
rm /media/dd_usb/cache_wget/192.168.0.102/chauffage_config.php
rm /media/dd_usb/cache_wget/192.168.0.102/conf_externe.php
rm /media/dd_usb/cache_wget/192.168.0.102/ventilation_flux.php
rm /media/dd_usb/cache_wget/192.168.0.102/webcam.php
rm /media/dd_usb/cache_wget/192.168.0.102/map_tondeuse.php
rm /media/dd_usb/cache_wget/192.168.0.102/planning.php
rm /media/dd_usb/cache_wget/192.168.0.102/reseau.php
rm /media/dd_usb/cache_wget/192.168.0.102/restemperature_stat.php

#on charge les pages "cas particuliers
#wget -r http://192.168.0.102:80
wget http://0.0.0.0:80/index.php
wget http://0.0.0.0:80/teleinfo_edf.php
wget http://0.0.0.0:80/voletroulant.php
wget http://0.0.0.0:80/pellet.php
wget http://0.0.0.0:80/chauffage_config.php
wget http://0.0.0.0:80/conf_externe.php
wget http://0.0.0.0:80/ventilation_flux.php
wget http://0.0.0.0:80/webcam.php
wget http://0.0.0.0:80/map_tondeuse.php
wget http://0.0.0.0:80/planning.php
wget http://0.0.0.0:80/temperature_stat.php

#On supprimer l'image du sejour (vie privee!!!)
rm /media/dd_usb/cache_wget/192.168.0.102/webcam/sejour.jpg

# upload des fichiers sur le site distant
echo upload du site ...
lftp ftp://$LOGIN:$PASSWORD@ftpperso.free.fr -e "mirror -R /media/dd_usb/cache_wget/192.168.0.102/ /static_domini  ; quit"

#suppression des fichiers temporaires
echo supression des fichiers temporaires ...
rm /media/dd_usb/cache_wget/192.168.0.102/*
rm -r /media/dd_usb/cache_wget/192.168.0.102/csv
rm -r /media/dd_usb/cache_wget/192.168.0.102/css
rm -r /media/dd_usb/cache_wget/192.168.0.102/webcam
