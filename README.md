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

#Captures d'écran / Screenshots
![Domini Ecran accueil](docs/img/screenshot/web_accueil.png)

![Domini Temperatures](docs/img/screenshot/web_temperature.png)

![Domini Consommation electrique live](docs/img/screenshot/web_elec_live.png)

![Domini Consommation electrique mensuelle](docs/img/screenshot/web_elec_mois.png)

![Domini Ensoleillement mensuel](docs/img/screenshot/web_soleil_mois.png)


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
- Base de données : MySQL (sudo apt-get install mysql-server) , mot de passe superadmin : mysql
- PHP (sudo apt-get install php) , à vérifier si DoMini est compatible à php7
- serveur FTP : vsftp (pour récupération images des cameras IP),
- GCC (pour compiler les applications faites en C)
- GD (lib graphique pour HTML) , à confirmer
*Il existe certainement des dépendances ...*


Pour le développement, mise au point ou confort, j'utilise également :

- Samba
- Webmin
- Git (sudo apt-get install)
- screen


#Notes pour l'installation ou la migration vers un nouveau PC

## pb de boot sur ub1610 (perte de config?)
	si au reboot : Reboot and select proper boot device or Insert Boot Media in selected boot device and press any key
	vérifier la config dans le bios ,
	n'autoriser le boot que sur le disque SSD interne (pas sur le samsung de 256Go)
	désactiver les boot2, boot3 etc... ne conserver que le boot0 sur le disque flash interne



## screen
commande pour relancer un screen existant
	screen -r -d -h 10000
	
pour avoir une status bar , éditer .screenrc avec ces lignes
	caption always
	caption string "%{=b kg} [%n %t] %{=s ky}%W  %= %{=b}%H %{=s}%c "

nécessite un redémarrage du pc (ou du service cron uniquement peut etre)
	

## GCC 
Installation :
	sudo apt install gcc
Installation lib mysql pour compiler sans erreur
	sudo apt-get install libmysqlclient-dev 

## Git
(rappel ici : https://git-scm.com/book/fr/v1/Les-bases-de-Git-D%C3%A9marrer-un-d%C3%A9p%C3%B4t-Git)
Rapatrier tout le projet
	se mettre dans ~src/ (ne pas créer de sous repertoire domini)
	git clone git://github.com/minbiocabanon/DoMini domini
	
Paramétrage du git
	git remote set-url origin https://github.com/minbiocabanon/DoMini
	git config --global user.email "minbiocabanon@gmail.com"
	git config --global user.name "minbiocabanon"

## INTERFACE WEB
Faire un lien symbolique de ~/serveur/www/domini vers /var/www/domini

	ln -s ~/serveur/www/domini /var/www/domini

**ATTENTION** : il ne faut pas avoir créé /var/www/domini avant de créér le lien symbolique

Changer les droits, le propriétaire de /var/www/domini doit etre www-data

	sudo chown -R www-data:www-data domini

### serveur web : lighttpd
installation :
	sudo apt-get install vsftpd
	
paramétrage lighttpd
	Changer dans /etc/lighttpd/lighttpd.conf le chemin  par défaut par /var/www/domini
	redémarre lighttpd : sudo /etc/init.d/lighttpd restart
	
	sudo lighty-enable-mod fastcgi 
	sudo lighty-enable-mod fastcgi-php
	/etc/init.d/lighttpd force-reload


	attention aux droits pour lancer/arrêter lighttpd, il faut être root + admin
	Un truc : dès qu'on installe des extensions, il faut killer httpd et les fastcgi et les fpm
	Puis relancer lighttpd	

**ATTENTION** : certaines pages PHP lancent des executables (copiés dans www/domini/bin lors de la compilation). L'executable 'emitter' accède au port série /dev/ttyUSB0 pour transmettre les ordres au Jeelink. Il est possible que l'utilisateur www-data n'est pas la permission pour accéder au port série.
	
	#la commande suivante permet de donner accés en lecture/écriture à tous les groupes
	sudo chmod 666 /dev/ttyUSB0

### Webmin
Download deb package:
	wget sourceforge.net/projects/webadmin/files/webmin/1.820/webmin_1.820_all.deb
Install deb package on ubuntu system :
	sudo dpkg -i --force-depends webmin_1.820_all.deb
Install complete depedencies on ubuntu system :
	sudo apt-get install -f

After installation is completed, open your browser and user this link address https://localhost:10000/ to login webmin

Login as username is your ubuntu accout name and password account

Vous pouvez utiliser votre nom d'utilisateur courant et mot de passe, mais si vous voulez utiliser le compte "root" de webmin, celui-ci sera inaccessible car désactivé sur Ubuntu. Il faut par conséquent le changer en tapant :

	sudo /usr/share/webmin/changepass.pl /etc/webmin root votre_mot_de_passe

	Cette commande ne change pas le mot de passe « root » d'Ubuntu mais seulement celui de Webmin.
Redémarrer le service webmin :
	sudo service webmin restart


To remove all webmin system :
	sudo apt-get remove webmin	
	
### BOOTSTRAP
le framework [bootstrap](http://getbootstrap.com/) est utilisé pour la création des pages web.
Cela permet d'avoir une IHM 'responsive', c'est à dire qui s'adapte en fonction du support (PC, smartphone, tablette...).
Il existe pleins de plugins pour bootstrap afin de compléter l'ergonomie de l'interface web : boutons, slider, calendrier, curseurs ...

### HIGHSTOCK
J'ai utilisé [Highstock](http://www.highcharts.com/) pour générer les graphiques. basé sur du javascript, les graphiques sont bien faits et personnalisables à volonté.
	
## BASE DE DONNEES
la base de données tourne avec MySQL.

Fichier contenant la structure de la base :

	~/serveur/bdd/struct_domotique.sql

créer la base:
	echo "create database domotique" | mysql -u root -p
	
Fichier contenant l'exportation de toutes les données (non disponible sur le dépot git car du domaine privé)
L'importation de gros fichiers n'est pas possible via phpmyadmin, il faut utiliser mysql en ligne de commande:

 	mysql --user=root --password=mysql domotique < 	 ~/serveur/bdd/backup_domotique/backup-domotique.sql

### phpmyadmin
	apt-get install phpmyadmin
	mdp : mysql
	
	
## SYSTEME
### Couleurs TERM
Pour avoir la console en couleur
	export TERM=xterm-color
	

### rc.local
Afin de garantir le fonctionnement de la domotique en cas de redémarrage intempestif du serveur (si pas d'onduleur ou à la reprise du courant lorsque l'onduleur est sec), il convient d'appeler un script 'go.sh' qui contiendra les actions à lancer au démarrage.
Pour cela, il faut modifier le fichier rc.local en ajoutant ces quelques lignes à la fin du fichier (avant exit 0) :

	éditer le fichier (avec nano ou vi):
	nano /etc/rc.local
	echo "running go.sh"
	/home/julien/src/domini/serveur/go.sh


Ubuntu utilise maintenant systemd et rc.local est maintenant considéré comme un service, qui est arrêté par défaut.

Il est possible de l’activer avec la commande 
	sudo systemctl enable rc-local.service
Il est nécessaire ensuite de redémarrer.

Pour le désactiver : 
	sudo systemctl disable rc-local.service

Pour voir si il est activé :
	systemctl list-unit-files | grep rc
	
	
### Accès au portCOM/USB
Le port ttyUSBx doit être utilisable par l'utilisateur www-data pour l'envoi de message directement depuis l'interface web.

	usermod -a -G dialout www-data
	ou
	adduser www-data dialout

Si vous lancez pyReceiver depuis votre compte user, ajoutez votre compte également de la même façon pour avoir le droit d'utiliser le port serie.

### Crontab:
Importer les taches CRON listées dans ce fichier :

	~/serveur/systeme/crontab.txt
	
Commande à éxécuter :

	crontab crontab.txt

### Users
L'utilisateur www-data doit disposer d'un mot de passe pour être compatible avec la configuration de mes caméras. Les caméras envoient par FTP des images dans le répertoire du serveur web.

Par défaut www-data ne possède pas de mot de passe. Lui donner "www-data" comme mot de passe à l'aide de la commande :

	sudo passwd www-data

###Connexions réseaux

Pour éviter que le wlan0 ne se déconnecte lorsque eth0 est débranché, dans /etc/network/interfaces , ne conserver 'auto' que devant le wlan0 (si wlan0) doit etre la liaison lan par défaut.
Configuration WEP pour interface wifi, voir ci-dessous.
	# cat /etc/network/interfaces
	
	# The loopback network interface
	#auto lo
	  iface lo inet loopback

	#auto eth0
	  iface eth0 inet dhcp

	auto wlan0
	  iface wlan0 inet dhcp
	   wireless-essid VIRUS
	   wireless-key s:"0000"


	   
	
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
	
	
### client lftp
installer un client FTP light pour
	sudo apt install lftp
	
### Serveur vsFTP
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

	
## Logiciel
 - Modifier le fichier suivant pour y renseigner les identifiants de connexion à la base de données ainsi que le nom de la table:

	~/serveur/include/passwd.h (nommé passwd.h_ qu'il faut renommer)
	
 - Recompiler tous les logiciels directement sur la cible avec la commande 'make'
 

### cURL

Installer le paquet cURL pour PHP (optionnel)

	sudo apt-get install php5-curl

# Python
## Pyserial
http://pypi.python.org/pypi/pyserial

télécharger le source, puis
	tar zxvf pyserial-3.2.1.tar.gz
	cd pyserial-3.2.1
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
	
##pushbullet python

installer ez-python :
télécharger ez-setup.py ici : https://pypi.python.org/pypi/setuptools	

	sudo python ez_setup.py

Cloner le github pushbullet.py en local 
	mkdir ~/download/pypushbullet
	cd ~/download/pypushbullet
	git clone https://github.com/randomchars/pushbullet.py
	
dans le répertoire , lancer :
	$ sudo python setup.py install
	
	

## Gestion onduleur USB (brouillon)

Voir dans le repertoire domini/serveur/ups pour plus d'info (readme.md)

liens utiles

https://doc.ubuntu-fr.org/nut

	$dmesg
	[ 4877.404023] usb 2-2: new low-speed USB device number 2 using uhci_hcd
	[ 4879.510653] usb 2-2: New USB device found, idVendor=0463, idProduct=ffff
	[ 4879.510659] usb 2-2: New USB device strings: Mfr=1, Product=2, SerialNumber=4
	[ 4879.510663] usb 2-2: Product: Protection Station
	[ 4879.510667] usb 2-2: Manufacturer: EATON
	[ 4879.510670] usb 2-2: SerialNumber: AN2E49008
	[ 4881.710163] generic-usb 0003:0463:FFFF.0004: hiddev0,hidraw2: USB HID v10.10 Device [EATON Protection Station] on usb-0000:00:1d.0-2/input0

##ddns
pour faire un ddns avec no-ip
	sudo curl -O https://www.noip.com/client/linux/noip-duc-linux.tar.gz
	sudo tar xzf noip-duc-linux.tar.gz
	cd noip-2.1.9-1/
	sudo make
	sudo make install
	#lancer le processus en fond de tâche
	sudo /usr/local/bin/noip2  #fonctionne pour le premier lancement
	sudo /usr/local/bin/noip2  -C #pour configurer un autre compte , il faudra arrêter le noip2
	
	
	# pour avoir le status du noip2 en cours :
	$sudo /usr/local/bin/noip2 -S
	1 noip2 process active.

	Process 26079, started as noip2, (version 2.1.9)
	Using configuration from /usr/local/etc/no-ip2.conf
	Last IP Address set 46.22.90.241
	Account ridezebigone@gmail.com
	configured for:
			host  kitemeteo.ddns.net
	Updating every 30 minutes via /dev/wlan0 with NAT enabled.
	
	# configuration pour 2 comptes
	$sudo /usr/local/bin/noip2 -C

	Auto configuration for Linux client of no-ip.com.

	Please enter the login/email string for no-ip.com  ridezebigone@gmail.com
	Please enter the password for user 'ridezebigone@gmail.com'  ************

	2 hosts are registered to this account.
	Do you wish to have them all updated?[N] (y/N)  y
	Please enter an update interval:[30]  30
	Do you wish to run something at successful update?[N] (y/N)  N

	New configuration file '/usr/local/etc/no-ip2.conf' created.

	# verification de l'activation des deux comptes no-ip
	$sudo /usr/local/bin/noip2 -S
	No noip2 processes active.

	Configuration data from /usr/local/etc/no-ip2.conf.
	Account ridezebigone@gmail.com
	configured for:
			host  kitemeteo.ddns.net
			host  mydomini.ddns.net
	Updating every 30 minutes via /dev/wlan0 with NAT enabled.

