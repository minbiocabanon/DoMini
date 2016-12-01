#!/bin/bash
cd /var/www/domini/php/
# calcul de la puissance du puits canadien
# suppression fichier temporaire
curl 0.0.0.0:80/php/puissance_pc_calc.php
curl 0.0.0.0:80/php/test_internet.php

#lancement des page PHP qui vont generer les graphs
cd /var/www/domini/php/highstock/

# GRAPHIQUE HIGHSTOCK
# G�n�re le CSV avec les info sonde int�rieure
curl 0.0.0.0:80/php/highstock/csv_temp_ext.php
# G�n�re le CSV avec les info sonde ext�rieure
curl 0.0.0.0:80/php/highstock/csv_temp_int.php
# G�n�re le CSV avec les info sonde du puits canadien
curl 0.0.0.0:80/php/highstock/csv_temp_pc.php
# G�n�re le CSV avec les info sonde sur une bouche d'insuflation air neuf
curl 0.0.0.0:80/php/highstock/csv_temp_air_neuf.php
# G�n�re le CSV avec les info sonde plac�e dans les combles
curl 0.0.0.0:80/php/highstock/csv_temp_comble.php
# G�n�re le CSV avec les info sonde de la VMC : air ext�rieur
curl 0.0.0.0:80/php/highstock/csv_temp_vmc_ext.php
# G�n�re le CSV avec les info de conso �lectrique instantann�e
curl 0.0.0.0:80/php/highstock/csv_teleinfo_live.php
# G�n�re le CSV avec les info de connexion internet
curl 0.0.0.0:80/php/highstock/csv_test_internet.php



