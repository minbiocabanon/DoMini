#!/bin/bash
cd /var/www/domini/webcam/

# capture l'image de la webcam, sortie sous le nom ext.jpg
wget -O ./ext.jpg http://user:user@192.168.0.116/axis-cgi/jpg/image.cgi

# capture l'image de la webcam SEJOUR, sortie sous le nom sejour.jpg
wget -O ./porche.jpg http://user:user@192.168.0.117/axis-cgi/jpg/image.cgi

# capture l'image de la webcam du garage sortie sous le nom garage.jpg
wget -O ./garage.jpg "http://192.168.0.121:88/cgi-bin/CGIProxy.fcgi?usr=user&pwd=user&cmd=snapPicture2"

# Capture webcam HTC Wildfire S -> image pleine resolution avec autofocus
wget -O ./sejour_full.jpg http://user:user@192.168.0.115:8080/photoaf.jpg

# on redimensionne l'image 
wget http://0.0.0.0:80/webcam/redim.php
# suppression des fichiers tampons
rm -f /var/www/domini/webcam/*.php.*


