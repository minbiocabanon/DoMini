#!/bin/bash
cd /var/www/domini/webcam/

# webcam du garage sortie sous le nom kiwi.jpg
curl --url "http://192.168.0.121:88/cgi-bin/CGIProxy.fcgi?usr=user&pwd=user&cmd=snapPicture2" --output ./kiwi.jpg

# on redimensionne l'image
#curl http://0.0.0.0:80/webcam/redim_garage.php

