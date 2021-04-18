#!/bin/bash
cd /var/www/domini/webcam/

# capture l'image de la webcam, sortie sous le nom ext.jpg
#echo curl 192.168.0.116
#curl --url http://user:user@192.168.0.116/axis-cgi/jpg/image.cgi --output ./kiwi.jpg


# capture l'image du garage
echo capture ffmpeg 192.168.0.119:554/live0.264
ffmpeg -y -rtsp_transport tcp -i 'rtsp://192.168.0.119:554/live0.264' -f singlejpeg -vframes 1 garage.jpg

# capture l'image jardin/terrasse
echo capture ffmpeg 192.168.0.120:554/live0.264
ffmpeg -y -rtsp_transport tcp -i 'rtsp://192.168.0.120:554/live0.264' -f singlejpeg -vframes 1 ext.jpg

# capture l'image de la webcam du garage sortie sous le nom garage.jpg
# echo curl 192.168.0.121
curl --url "http://192.168.0.121:88/cgi-bin/CGIProxy.fcgi?usr=user&pwd=user&cmd=snapPicture2" --output ./garage_full.jpg

# capture l'image du porche
echo capture ffmpeg 192.168.0.129:554/live0.264
#curl --url http://user:user@192.168.0.117/axis-cgi/jpg/image.cgi --output ./porche.jpg
ffmpeg -y -rtsp_transport tcp -i 'rtsp://192.168.0.129:554/live0.264' -f singlejpeg -vframes 1 porche.jpg


# Capture Foscam C2 sejour
echo curl 192.168.0.130
curl --url "http://192.168.0.130:88/cgi-bin/CGIProxy.fcgi?cmd=snapPicture2&usr=user&pwd=userFoscamC2" --output ./sejour_full.jpg


# on redimensionne les images
curl http://0.0.0.0:80/webcam/redim.php



