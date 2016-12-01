#!/bin/bash
#lancement des page PHP qui vont generer les graphs
cd /var/www/domini/php/highstock/
# GRAPHIQUE HIGHSTOCK
# Génére le CSV avec les info sonde intérieure
curl 0.0.0.0:80/php/highstock/csv_puissance_pc.php


