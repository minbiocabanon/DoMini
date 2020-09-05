#!/bin/bash
cd /var/www/domini/php/
# charge les données temps réelles dans une table temporaire -> permet d'accélerer l'affichage de la page d'accueil de l'interface domotique
curl 0.0.0.0:80/php/instant_data.php

curl 0.0.0.0:80/php/test_internet.php

#lancement des page PHP qui vont generer les graphs
cd /var/www/domini/php/highstock/

# Génére le CSV avec les info de connexion internet
curl 0.0.0.0:80/php/highstock/csv_test_internet.php

