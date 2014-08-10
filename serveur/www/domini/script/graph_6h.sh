#!/bin/bash
#lancement des page PHP qui vont generer les graphs
cd /var/www/domini/php/highstock/
# GRAPHIQUE HIGHSTOCK
# Génére le CSV avec les info sonde intérieure
wget 0.0.0.0:80/php/highstock/csv_puissance_pc.php

#suppression des fichiers temporaire crees
rm -f /var/www/domini/php/highstock/csv_puissance_pc.php.*


