#!/bin/bash
#lancement des page PHP qui vont generer les graphs
cd /var/www/domini/php/highstock/
# GRAPHIQUE HIGHSTOCK
# G�n�re le CSV avec les info sonde int�rieure
curl 0.0.0.0:80/php/highstock/csv_puissance_pc.php


