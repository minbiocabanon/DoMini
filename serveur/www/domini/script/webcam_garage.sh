#!/bin/bash
cd /var/www/domini/webcam/

# webcam du garage sortie sous le nom kiwi.jpg
#curl --url "http://192.168.0.121:88/cgi-bin/CGIProxy.fcgi?usr=user&pwd=user&cmd=snapPicture2" --output ./kiwi.jpg

# capture l'image du garage
echo capture ffmpeg 192.168.0.119:554/live0.264
/usr/local/bin/ffmpeg -y -rtsp_transport tcp -i 'rtsp://192.168.0.119:554/live0.264' -f singlejpeg -vframes 1 kiwi.jpg

# on redimensionne l'image
#curl http://0.0.0.0:80/webcam/redim_garage.php

