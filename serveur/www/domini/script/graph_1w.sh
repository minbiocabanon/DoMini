#!/bin/bash
#lancement des page PHP qui vont generer les graphs
cd /var/www/domini/php/highstock/

# GRAPHIQUE HIGHSTOCK
# Génére le CSV avec les info téléinfo annuelle
curl 0.0.0.0:80/php/highstock/csv_teleinfo_an.php
curl 0.0.0.0:80/php/highstock/csv_pellet_conso.php
curl 0.0.0.0:80/php/highstock/csv_pellet_conso_par_hiver.php
