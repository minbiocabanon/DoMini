Domini
======

Mon projet domotique / My Home automation project

((BROUILLON))
======

#Intro


![Domini architecture](/docs/Synoptique_DoMini.png)


#Pré-requis

##Plateforme
Le serveur de la domotique est une machine Linux. 

Le projet ne nécessite que peu de ressources, jusqu'en aout 2014 il tournait sur un **Sheevaplug** :

- *Linux DEBIAN 6 Lenny*
- *ARM v5 1.2GHz*
- *RAM 512 Mo*
- *Flash 512 Mo* 
- *Lecteur SDCard (remplacé au bout de 6 mois par un disque dur USB car la SDCard ne supportait pas les nombreuses écritures)*
- *1 port USB*
- *1 port Ethernet*

Le projet Domini ayant atteint une certaine maturité et ayant acquis une place presque indispensable dans la maison (gestion du chauffage, volets et bientôt l'alarme) j'ai voulu fiabilisé le serveur.
Pour cela, j'ai migré du sheevaplug vers un **PC Barebone** (mon ancien PC recyclé) avec des disques dur montés en RAID1 et avec un peu plus de puissance pour réaliser les tâches (serveur web, sauvegarde base de données,...).

Voici les caractéristiques du PC (à titre d'information, cela n'indique pas le minimum nécessaire)

- *Linux DEBIAN 7*
- *PC x86 : Intel P4*
- *RAM : 2Go*
- *Disque dur : 2 x 80 Go ( montés en RAID1)*
- *1 port USB*
- *Wifi b/g/n*
- *Reseau Ethernet 10/100Mbps (optionnel une fois le wifi installé)*

#Paquets et logiciels

La liste des paquets installés sur ma machine est disponible sur le dépôt :

	~/serveur/systeme/dpkg.txt

Dans les grandes lignes voici ce qui est essentiel pour le projet :

- Serveur web : lighttpd
- Base de données : MySQL
- PHP
- GD (lib graphique pour HTML)
- GCC (pour compiler les applications faites en C)
*Il existe certainement des dépendances ...*


Pour le développement, mise au point ou confort, j'utilise également :

- Samba
- Webmin
- Git
- screen



#Installation

## SAMBA
pour faciliter l'accès aux fichiers depuis le réseau, mettre / en accès (attention si l'accès est en root!!!)


## BASE DE DONNEES

L'importation de gros fichiers n'est pas possible via phpmyadmin

utiliser mysql en ligne de commande
 mysql --user=root --password=mysql domotique < /home/julien/src/SHEEVA_SERVER/bdd/backup_domotique/backup-domotique.sql
 
 
## INTERFACE WEB

faire un lien symbolique de 

~/serveur/www/domini	vers /var/www/domini

ajouter le multihost dans lighttp.conf
		 ### Ajout virtual host Webcam ###
		$SERVER["socket"] == ":82" {
		 server.document-root = "/var/www/webcam/"
		 server.errorlog = "/var/log/lighttpd/webcam/error.log"
		 }

		 
## SYSTEME
## Crontab:
	
	Importer les taches CRON listées dans ce fichier :
	~/serveur/systeme/crontab.txt
	
	Commande à éxécuter :
	crontab crontab.txt
