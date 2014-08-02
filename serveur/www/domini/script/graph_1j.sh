#!/bin/bash

#copie de l'archive de la base de données sur le disque dur
cp /root/domini/bdd/backup_domotique/domotique.tgz /media/dd_usb/archive_bdd/domotique.tgz

#lancement des page PHP qui vont generer les graphs
cd /var/www/domini/php/highstock/

# teleinfo -> ajoute uniquement au fichier CSV les données de conso de la veille
wget 0.0.0.0:80/php/highstock/csv_teleinfo_jour_LIGHT.php
# pellet -> on met à jour le CSV avec les conso de granulés
wget 0.0.0.0:80/php/highstock/csv_pellet.php
# ensoleillement mensuel
wget 0.0.0.0:80/php/highstock/csv_pyrano_mensuel.php

#suppression des fichiers temporaire crees
rm /var/www/domini/php/highstock/csv_teleinfo_jour_LIGHT.php.* 
rm /var/www/domini/php/highstock/csv_pellet.php.* 
rm /var/www/domini/php/highstock/csv_pyrano_mensuel.php.*

