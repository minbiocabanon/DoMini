#!/bin/bash
cd /var/www/domini/php/
# charge les données temps réelles dans une table temporaire -> permet d'accélerer l'affichage de la page d'accueil de l'interface domotiqut
curl 0.0.0.0:80/php/instant_data.php



