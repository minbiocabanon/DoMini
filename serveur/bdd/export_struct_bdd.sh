#!/bin/sh
#sauvegarde de la structure des bases de donnees
cd /media/dd_usb/SHEEVA_SERVER/bdd
mysqldump --user=root --password=mysql -d --opt domotique > struct_domotique.sql;

echo base domotique exportee

