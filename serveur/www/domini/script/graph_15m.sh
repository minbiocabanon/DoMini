#!/bin/bash
cd /var/www/domini/php/
# calcul de la puissance du puits canadien
# suppression fichier temporaire
wget 0.0.0.0:80/php/puissance_pc_calc.php
wget 0.0.0.0:80/php/test_internet.php
rm -f /var/www/domini/php/puissance_pc_calc.php.*
rm /var/www/domini/php/test_internet.php.*

#lancement des page PHP qui vont generer les graphs
cd /var/www/domini/php/highstock/

# GRAPHIQUE HIGHSTOCK
# Génére le CSV avec les info sonde intérieure
wget 0.0.0.0:80/php/highstock/csv_temp_ext.php
# Génére le CSV avec les info sonde extérieure
wget 0.0.0.0:80/php/highstock/csv_temp_int.php
# Génére le CSV avec les info sonde du puits canadien
wget 0.0.0.0:80/php/highstock/csv_temp_pc.php
# Génére le CSV avec les info sonde sur une bouche d'insuflation air neuf
wget 0.0.0.0:80/php/highstock/csv_temp_air_neuf.php
# Génére le CSV avec les info sonde placée dans les combles
wget 0.0.0.0:80/php/highstock/csv_temp_comble.php
# Génére le CSV avec les info sonde de la VMC : air extérieur
wget 0.0.0.0:80/php/highstock/csv_temp_vmc_ext.php
# Génére le CSV avec les info de conso électrique instantannée
wget 0.0.0.0:80/php/highstock/csv_teleinfo_live.php
# Génére le CSV avec les info de connexion internet
wget 0.0.0.0:80/php/highstock/csv_test_internet.php


#suppression des fichiers temporaire crees
rm -f /var/www/domini/php/highstock/csv_temp_ext.php.*
rm -f /var/www/domini/php/highstock/csv_temp_int.php.*
rm -f /var/www/domini/php/highstock/csv_temp_pc.php.*
rm -f /var/www/domini/php/highstock/csv_temp_air_neuf.php.*
rm -f /var/www/domini/php/highstock/csv_temp_comble.php.*
rm -f /var/www/domini/php/highstock/csv_temp_vmc_ext.php.*
rm -f /var/www/domini/php/highstock/csv_teleinfo_live.php.*
rm -f /var/www/domini/php/highstock/csv_test_internet.php.*


