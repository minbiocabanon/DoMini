**ATTENTION**
Pour les scripts situés dans /var/www/domini/script/ , veillez à ce que ce soit l'utilisation www-data:www-date qui éxécute les tâches CRON

*NOTES*

Il est possible de lister l'ensemble des paquetages installés grâce à la commande : 
dpkg --get-selections

Grâce à cet outil il est ainsi possible d'exporter la liste des paquetages installés de la manière suivante : 
dpkg --get-selections > mes_paquetages

Puis de les installer avec la commande suivante sur une autre machine : 

Récupération de la liste précédente : 
dpkg --set-selections < mes_paquetages

Installation de la liste : 
apt-get dselect-upgrade



La commande dpkg -l retourne la liste des paquets installés avec plus d'informations.
 Cependant il n'est pas possible de l'utiliser pour installer une liste de paquets.
