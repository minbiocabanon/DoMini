#!/bin/bash
#lancement des page PHP qui vont generer les graphs
cd /var/www/domini/php/highstock/

# GRAPHIQUE HIGHSTOCK
# Génére le CSV avec les info téléinfo annuelle
wget 0.0.0.0:80/php/highstock/csv_teleinfo_an.php
wget 0.0.0.0:80/php/highstock/csv_pellet_conso.php

#suppression des fichiers temporaire crees
rm /var/www/domini/php/highstock/csv_teleinfo_an.php.*
rm /var/www/domini/php/highstock/csv_pellet_conso.php.*


