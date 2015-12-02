Domini
======

Mon projet domotique / My Home automation project

#ToDo

- trouver un moyen de ne pas mettre en dur les chemins dans les scripts de crontab ou certains scripts
- possibilité de mettre le chemin d'install du projet dans une variable puis d'utiliser cette variable dans les scripts pour travailler en relatif ?


#Demo
A l'adresse suivante, une version 'demo' de l'interface web de ma domotique est visible.
http://minbiocabanon.free.fr/static_domini/
*Attention : les actions (boutons) et certaines pages renverront des erreurs car il n'y a pas de base de données qui tourne pour la démo. Il s'agit uniquement d'une 'photo instantanée' de mes données et interface que je mets à jour régulièrement.*



#Intro
Architecture de l'installation :
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
- serveur FTP : vsftp (pour récupération images des cameras IP)
- GCC (pour compiler les applications faites en C)
*Il existe certainement des dépendances ...*


Pour le développement, mise au point ou confort, j'utilise également :

- Samba
- Webmin
- Git
- screen



#Notes pour l'installation ou la migration vers un nouveau PC

## BASE DE DONNEES
la base de données tourne avec MySQL.

Fichier contenant la structure de la base :

	~/serveur/bdd/struct_domotique.sql
	
Fichier contenant l'exportation de toutes les données (non disponible sur le dépot git car du domaine privé)
L'importation de gros fichiers n'est pas possible via phpmyadmin, il faut utiliser mysql en ligne de commande:

 	mysql --user=root --password=mysql domotique < 	 ~/serveur/bdd/backup_domotique/backup-domotique.sql
 
## SYSTEME
### rc.local
Afin de garantir le fonctionnement de la domotique en cas de redémarrage intempestif du serveur (si pas d'onduleur ou à la reprise du courant lorsque l'onduleur est sec), il convient d'appeler un script 'go.sh' qui contiendra les actions à lancer au démarrage.
Pour cela, il faut modifier le fichier rc.local en ajoutant ces quelques lignes à la fin du fichier (avant exit 0) :

	éditer le fichier (avec nano ou vi):
	nano /etc/rc.local
	echo "running go.sh"
	/home/julien/src/domini/serveur/go.sh

### Accès au portCOM/USB
Le port ttyUSBx doit être utilisable par l'utilisateur www-data pour l'envoi de message directement depuis l'interface web.

	usermod -a -G dialout www-data
	ou
	adduser www-data dialout

Si vous lancer pyReceiver depuis votre compte user, ajoutez votre compte également de la même façon pour avoir le droit d'utiliser le port serie.

### Crontab:
Importer les taches CRON listées dans ce fichier :

	~/serveur/systeme/crontab.txt
	
Commande à éxécuter :

	crontab crontab.txt

### Users
L'utilisateur www-data doit disposer d'un mot de passe pour être compatible avec la configuration de mes caméras. Les caméras envoient par FTP des images dans le répertoire du serveur web.

Par défaut www-data ne possède pas de mot de passe. Lui donner "www-data" comme mot de passe à l'aide de la commande :

	sudo passwd www-data
	
	
### vsFTP
Ce serveur FTP doit être installé si vous avez installé tous les paquets présents dans le fichier ~/serveur/systeme/dpkg.txt

La configuration qui fonctionne (dans mon cas) est celle-ci :

fichier de configuration /etc/vsftpd.conf :

	listen=YES
	anonymous_enable=YES
	local_enable=YES
	write_enable=YES
	anon_upload_enable=YES
	anon_mkdir_write_enable=YES
	dirmessage_enable=YES
	xferlog_enable=YES
	connect_from_port_20=YES
	chown_uploads=YES
	chown_username=ftp
	ftpd_banner=Welcome to blah FTP service.
	secure_chroot_dir=/var/run/vsftpd
	pam_service_name=vsftpd
	rsa_cert_file=/etc/ssl/certs/vsftpd.pem
	anon_root=/var/www/	

### resolv.conf
(pour résoudre pb de DNS si votre FAI est mauvais...)
installer le packet resolvconf
	sudo apt-get install resolvconf
	
éditer manuellement le fichier base :
	sudo nano /etc/resolvconf/resolv.conf.d/base
		
ajouter ces deux lignes :
	nameserver 208.67.222.222
	nameserver 208.67.220.220

sauver et relancer l'interface wifi/ethernet
	sudo /etc/init.d/networking restart

	
## Logiciel
 - Modifier le fichier suivant pour y renseigner les identifiants de connexion à la base de données ainsi que le nom de la table:

	~/serveur/include/passwd.h (nommé passwd.h_ qu'il faut renommer)
	
 - Recompiler tous les logiciels directement sur la cible avec la commande 'make'
 
## INTERFACE WEB
Faire un lien symbolique de ~/serveur/www/domini vers /var/www/domini

	ln -s ~/serveur/www/domini /var/www/domini

**ATTENTION** : il ne faut pas avoir créé /var/www/domini avant de créér le lien symbolique

**ATTENTION** : certaines pages PHP lancent des executables (copiés dans www/domini/bin lors de la compilation). L'executable 'emitter' accède au port série /dev/ttyUSB0 pour transmettre les ordres au Jeelink. Il est possible que l'utilisateur www-data n'est pas la permission pour accéder au port série.
	
	#la commande suivante permet de donner accés en lecture/écriture à tous les groupes
	sudo chmod 666 /dev/ttyUSB0

### BOOTSTRAP
le framework [bootstrap](http://getbootstrap.com/) est utilisé pour la création des pages web.
Cela permet d'avoir une IHM 'responsive', c'est à dire qui s'adapte en fonction du support (PC, smartphone, tablette...).
Il existe pleins de plugins pour bootstrap afin de compléter l'ergonomie de l'interface web : boutons, slider, calendrier, curseurs ...

### HIGHSTOCK
J'ai utilisé [Highstock](http://www.highcharts.com/) pour générer les graphiques. basé sur du javascript, les graphiques sont bien faits et personnalisables à volonté.

### Captures d'écran / Screenshots
![Domini Ecran accueil](docs/img/screenshot/web_accueil.png)

![Domini Temperatures](docs/img/screenshot/web_temperature.png)

![Domini Consommation electrique live](docs/img/screenshot/web_elec_live.png)

![Domini Consommation electrique mensuelle](docs/img/screenshot/web_elec_mois.png)

![Domini Ensoleillement mensuel](docs/img/screenshot/web_soleil_mois.png)

### cURL

Installer le paquet cURL pour PHP (optionnel)

	sudo apt-get install php5-curl

# Python
## Pyserial
http://pyserial.sourceforge.net/pyserial.html#from-source-tar-gz-or-checkout

télécharger le source, puis
	tar zxvf pyserial-2.7.tar.gz
	cd pyserial-2.7
	sudo python setup.py install
	
lancer Python
	python

importer serial
	>>>import serial
	
##MySQL Python
Installer le paquet :
	sudo apt-get install python-mysqldb

Interpreteur PYTHON :

	>>> import MySQLdb as mdb
	>>> con = mdb.connect('localhost','root','mysql','domotique')
	>>> cur = con.cursor()
	>>> cur.execute("select version()")
	1L
	>>> ver = cur.fetchone()
	>>> print "%s" % ver
	5.5.40-0+wheezy1
	>>>
	

## Gestion onduleur USB (brouillon)

liens utiles
http://eole.orion.education.fr/oldwiki/index.php/Installation_Onduleur
http://ovanhoof.developpez.com/upsusb/

	$dmesg
	[ 4877.404023] usb 2-2: new low-speed USB device number 2 using uhci_hcd
	[ 4879.510653] usb 2-2: New USB device found, idVendor=0463, idProduct=ffff
	[ 4879.510659] usb 2-2: New USB device strings: Mfr=1, Product=2, SerialNumber=4
	[ 4879.510663] usb 2-2: Product: Protection Station
	[ 4879.510667] usb 2-2: Manufacturer: EATON
	[ 4879.510670] usb 2-2: SerialNumber: AN2E49008
	[ 4881.710163] generic-usb 0003:0463:FFFF.0004: hiddev0,hidraw2: USB HID v10.10 Device [EATON Protection Station] on usb-0000:00:1d.0-2/input0
