#!/bin/bash
#lancement des page PHP qui vont generer les graphs
cd /var/www/domini/php/highstock/

# Génére le CSV avec les info de puissance récupérée par le puits canadien
curl 0.0.0.0:80/php/highstock/csv_chauffage_log.php
# Génére le CSV avec les info des consignes du bypass
curl 0.0.0.0:80/php/highstock/csv_consigne_pc.php
#log sur l'état des volets roulants
curl 0.0.0.0:80/php/highstock/csv_voletroulant_log.php
# Génére le CSV avec les info du flux solaire
curl 0.0.0.0:80/php/highstock/csv_pyrano.php
#Génére le CSV avec les infos de températures de la vmcdf
curl 0.0.0.0:80/php/highstock/csv_temp_vmc.php
# Génére le CSV avec les info du niveau de pellets dans le reservoir
curl 0.0.0.0:80/php/highstock/csv_pellet_rsv.php