#!/bin/bash
#Sauvegarde de la crontab
cd /media/dd_usb/SHEEVA_SERVER/systeme/
rm crontab*.txt
crontab -l > crontab_root.txt
crontab -u www-data -l > crontab_www-data.txt
cat crontab_root.txt crontab_www-data.txt > crontab.txt
rm crontab_*.txt
echo crontab exportée
