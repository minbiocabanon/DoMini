#!/bin/bash
#lancement des page PHP qui vont generer les graphs
cd /var/www/domini/php/highstock/

# G�n�re le CSV avec les info de puissance r�cup�r�e par le puits canadien
curl 0.0.0.0:80/php/highstock/csv_chauffage_log.php
# G�n�re le CSV avec les info des consignes du bypass
curl 0.0.0.0:80/php/highstock/csv_consigne_pc.php
#log sur l'�tat des volets roulants
curl 0.0.0.0:80/php/highstock/csv_voletroulant_log.php
# G�n�re le CSV avec les info du flux solaire
curl 0.0.0.0:80/php/highstock/csv_pyrano.php
#G�n�re le CSV avec les infos de temp�ratures de la vmcdf
curl 0.0.0.0:80/php/highstock/csv_temp_vmc.php
# G�n�re le CSV avec les info du niveau de pellets dans le reservoir
curl 0.0.0.0:80/php/highstock/csv_pellet_rsv.php