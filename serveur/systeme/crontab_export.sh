#!/bin/bash
#Sauvegarde de la crontab
cd /home/jcaulier/src/domini/serveur/systeme/
rm crontab*.txt
crontab -l > crontab_root.txt
crontab -u www-data -l > crontab_www-data.txt
crontab -u jcaulier -l > crontab_jcaulier.txt
cat crontab_root.txt crontab_www-data.txt crontab_jcaulier.txt > crontab.txt
rm crontab_*.txt
echo crontab exportee
