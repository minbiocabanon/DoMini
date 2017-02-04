#!/bin/bash
cd /var/www/domini/webcam/timelapse/

# capture l'image de la webcam, sortie sous le nom ext.jpg
curl --url "http://user:user@192.168.0.116/axis-cgi/jpg/image.cgi" --output ./timelapse_$(date +%s).jpg



