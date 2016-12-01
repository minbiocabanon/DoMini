#!/bin/bash

#copie de l'archive de la base de donn�es sur le disque dur
cp /root/domini/bdd/backup_domotique/domotique.tgz /media/dd_usb/archive_bdd/domotique.tgz

#lancement des page PHP qui vont generer les graphs
cd /var/www/domini/php/highstock/

# teleinfo -> ajoute uniquement au fichier CSV les donn�es de conso de la veille
curl 0.0.0.0:80/php/highstock/csv_teleinfo_jour_LIGHT.php
# pellet -> on met � jour le CSV avec les conso de granul�s
curl 0.0.0.0:80/php/highstock/csv_pellet.php
# ensoleillement mensuel
curl 0.0.0.0:80/php/highstock/csv_pyrano_mensuel.php

