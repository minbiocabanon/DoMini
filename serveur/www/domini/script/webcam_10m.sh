#!/bin/bash
cd /var/www/domini/webcam/

# capture l'image de la webcam, sortie sous le nom ext.jpg
echo curl 192.168.0.116
curl --url http://user:user@192.168.0.116/axis-cgi/jpg/image.cgi --output ./ext.jpg

# capture l'image de la webcam SEJOUR, sortie sous le nom sejour.jpg
echo curl 192.168.0.117
#curl --url http://user:user@192.168.0.117/axis-cgi/jpg/image.cgi --output ./porche.jpg
curl --url http://192.168.0.125/snapshot.jpeg --output ./porche.jpg

# capture l'image de la webcam du garage sortie sous le nom garage.jpg
echo curl 192.168.0.121
curl --url "http://192.168.0.121:88/cgi-bin/CGIProxy.fcgi?usr=user&pwd=user&cmd=snapPicture2" --output ./garage_full.jpg

# Capture Foscam C2 sejour
echo curl 192.168.0.130
curl --url "http://192.168.0.130:88/cgi-bin/CGIProxy.fcgi?cmd=snapPicture2&usr=user&pwd=userFoscamC2" --output ./sejour_full.jpg

# capture l'image de la webcam PORCHE, sortie sous le nom proche2.jpg
echo curl 192.168.0.129
curl --url http://192.168.0.129/cgi-bin/getsnapshot.cgi --output ./porche2.jpg

# Capture webcam HTC Wildfire S -> image pleine resolution avec autofocus
#curl --url http://user:user@192.168.0.115:8080/photoaf.jpg --output ./sejour_full.jpg

# on redimensionne l'image 
curl http://0.0.0.0:80/webcam/redim.php



