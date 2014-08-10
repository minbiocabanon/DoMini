#!/bin/sh
#include pour les login et mot de passe du FTP. (ce fichier n'est pas sur le dépot SVN pour la confidentialité)
installpath=../www/domini/script/
. $installpath/myvar.sh

# on fait un backup de la base domotique
mysqldump --user=root --password=mysql --opt domotique > backup-domotique.sql
echo base domotique sauvee

#on compresse le fichier
tar cvzf domotique.tgz backup-domotique.sql
echo backup compresse

# on supprime le fichier temporaire
rm backup-domotique.sql
echo fichier .sql temporaire supprime

#On sauve l'archive sur un serveur distant... au cas ou le sheevaplus crame (ou la SDCard ou le Disque dur...)
# upload du fichier de sauvegarde sur le site distant
lftp ftp://$LOGIN:$PASSWORD@ftpperso.free.fr -e "cd /static_domini/archive_bdd; put domotique.tgz; quit"
echo fichier .tgz uploadé sur le site distant
