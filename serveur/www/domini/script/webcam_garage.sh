#!/bin/bash
cd /var/www/domini/webcam/

# capture l'image de la webcam du garage sortie sous le nom garage.jpg
curl --url "http://192.168.0.121:88/cgi-bin/CGIProxy.fcgi?usr=user&pwd=user&cmd=snapPicture2" --output ./garage_pepette.jpg

# on redimensionne l'image 
curl http://0.0.0.0:80/webcam/redim_garage.php



