#!/bin/bash
#lancement des page PHP qui vont generer les graphs

cd /var/www/domini/php/highstock/

# GRAPHIQUE HIGHSTOCK
# G�n�re le CSV avec les info t�l�info annuelle
curl 0.0.0.0:80/php/highstock/csv_teleinfo_mois_LIGHT.php


