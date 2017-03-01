Serveur
=====

Ce repertoire contient l'ensemble des logiciels et outils qui tournent sur le serveur domotique.

Plusieurs logiciels tournent de façon asynchrone et en parallèle afin d'éxécuter des tâches diverses.
La base de données sert à échanger des informations entre les différentes applications.

Les logiciels sont écrits en C. Certains sont juste des scripts shell (Linux).

##bdd
ce répertoire contient des scripts pour exporter la structure de la base de données, exporter la base pour archivage et des logiciels qui permettent de nettoyer les tables (tri, suppression intelligente des données inutiles, etc..)
Ces scripts ou application sont lancés périodiquement à partir de la CRONTAB

  ~/serveur/systeme/crontab.txt


##ephemeride
Actuellement, les éphémérides solaires (levé et couché du soleil) se résume à une table en 'dur' dans la BDD.
Précédemment, je calculais tous les matins les éphémérides mais les algos que j'avais trouvé n'étaient pas facile à gérer (fuseau horaire et mix avec changement d'heure qui buggait tout le temps). N'étant pas à la seconde près, j'ai finalement fait une table annuelle.
**ATTENTION** Les horaires correspondent à ma position géographique !


##jeelink
Ce répertoire contient le sketche (.ino) à programmer dans le jeelink.
Le jeelink transmet les messages au jeenodes et gère les ACK (si on l'application demande un ACK)

##include
ce répertoire contient le fichier **passwd.h** , fichier include nécessaire aux applications en langage C car il contient les identifiants pour la connexion à la base de données ainsi que le nom de la base.


##planning
Ce répertoire contient les tableurs dans lesquels je créée un CSV qui sera le planning annuel.
Ce CSV sera importé manuellement dans la table 'planning'.
Ensuite il faut lancer l'application planning qui va construire une table à partir de ce planning, des saisons et des types de jours spécifiés.

##pyBypassPC
Ce logiciel détermine la position du bypass du puits canadien à partir des températures.
Le logiciel est lancé périodiquement par la CRONTAB

  ~/serveur/systeme/crontab.txt

Un message est stocke dans la base à l'attention du jeenode qui gère le bypass. 
Le logiciel 'emitter' enverra ce message.

##pygestion_VR
Logiciel en python qui gère la position de volets roulants en fonction du soleil, de la saison etc...
Un message est stocké dans la base à l'attention du jeenode qui gère les VR. 
Le logiciel 'emitter' enverra ce message.

##pyNiveauGral
Logiciel en python qui surveille le niveau de granules dans le reservoir (msg envoyé par le poele puis stocké en bdd).
Une notification pushbullet sera transmise en cas d'alerte.

##pyReceiver
Edit : Programme en langage python
Ce logiciel gère la réception des messages reçus par le Jeelink et l'émission des messages placés dans la pile en base de données (MySQL)
En fonction du type de message, il dépouille et stocke les données dans la table correspondance dans la base de données.
**Ce logiciel doit tourner en permanence afin de ne pas rater de message!**

##pySentinelle
Programme en python qui surveille l'état du serveur web, de MySQL, de la connexion wifi etc... 
En cas d'alerte, envoi une notification pushbullet

##regul_temp
Logiciel qui gère les consignes à transmettre au poele pour réguler la température de la maison. Le planning, les températures et les consignes manuelles sont pris en compte, puis le logicie stocke un message (consigne de chauffe) dans la base à l'attention du jeenode qui gère le poele. 
Le logiciel 'emitter' enverra ce message.


##systeme
Ce répertoire contient les scripts et fichier relatifs au système :
 - script pour exporter la liste des paquets (éxécuter régulièrement par CRON)
 - script pour exporter le CRONTAB (éxécuter régulièrement par CRON)

##ups
Programme et module python qui surveille l'état de l'onduleur.
En cas de coupure de courant ou de rétablissement du courant, une notification pushbullet est envoyée.

##www
Ce répertoire contient toute l'interface graphique (web) de la domotique.
Un lien symbolique sera fait sur ce répertoire depuis /var/www/ .
