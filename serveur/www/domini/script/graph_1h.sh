#!/bin/bash
#lancement des page PHP qui vont generer les graphs
cd /var/www/domini/php/highstock/

# Génére le CSV avec les info de puissance récupérée par le puits canadien
wget 0.0.0.0:80/php/highstock/csv_chauffage_log.php
# Génére le CSV avec les info des consignes du bypass
wget 0.0.0.0:80/php/highstock/csv_consigne_pc.php
#log sur l'état des volets roulants
wget 0.0.0.0:80/php/highstock/csv_voletroulant_log.php
# Génére le CSV avec les info du flux solaire
wget 0.0.0.0:80/php/highstock/csv_pyrano.php
#Génére le CSV avec les infos de températures de la vmcdf
wget 0.0.0.0:80/php/highstock/csv_temp_vmc.php

#suppression des fichiers temporaire crees
rm /var/www/domini/php/highstock/csv_chauffage_log.php.*
rm /var/www/domini/php/highstock/csv_consigne_pc.php.*
rm /var/www/domini/php/highstock/csv_voletroulant_log.php.*
rm /var/www/domini/php/highstock/csv_pyrano.php.*
rm /var/www/domini/php/highstock/csv_temp_vmc.php.*