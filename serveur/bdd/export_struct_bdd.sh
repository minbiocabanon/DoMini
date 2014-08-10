#!/bin/sh
#on se place dans le répertoire
. domini_path.sh
cd $dominipath/serveur/bdd/

#sauvegarde de la structure des bases de donnees
mysqldump --user=root --password=mysql -d --opt domotique > struct_domotique.sql;

echo base domotique exportee

