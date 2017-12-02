Ubuntu : gere un onduleur :
	https://doc.ubuntu-fr.org/nut
	
# (§4.3 du site ubuntu )pour lancer les pilotes des onduleurs manuellement
	upsdrvctl start	

# gestion de l'onduleur par python
le logiciel ups_daemonized.py se lance en fond de tâche. 
Si besoin de le tuer faire :
	ps -x
relever le N° de PID, puis
	kill -9 [PID]
	
Le logiciel pyUps.py fait pareil mais sans daemon.

Dans les deux cas, des logs sont inscrit dans /var/log/syslog
	
	
# gestion de l'onduleur avec des commandes systemes	
## Obtenir la liste des onduleurs déclarés
Pour connaitre la liste des onduleurs déclarés sur la machine :

	upsc -L

	ou si l'onduleur est sur une machine distante ayant pour alias nom_de_la_machine_distante :

	upsc -L nom_de_la_machine_distante

## Connaître l'état d'un onduleur

	upsc eaton800
	
ou si l'onduleur est sur une machine distante :

	upsc myups@nom_de_la_machine_distante

Pour connaître un paramètre en particulier, ici le statut de l'onduleur3) :

upsc myups ups.status


## Envoyer une commande à l'onduleur
Pour connaître la liste des commandes supportées par le pilote et l'onduleur :

	upscmd -l myups@localhost

Par exemple pour le pilote megatec_usb :

	Instant commands supported on UPS [myups]:

	beeper.toggle - Toggle the UPS beeper
	load.off - Turn off the load immediately
	load.on - Turn on the load immediately
	reset.input.minmax - Reset minimum and maximum input voltage status
	reset.watchdog - Reset watchdog timer
	shutdown.return - Turn off the load and return when power is back
	shutdown.stayoff - Turn off the load and remain off
	shutdown.stop - Stop a shutdown in progress
	test.battery.start - Start a battery test
	test.battery.start.deep - Start a deep battery test
	test.battery.stop - Stop the battery test
	
Pour envoyer une commande,

	upscmd -u <username> -p <password> <system> <command>

Par exemple, pour demander un test de batterie sur l'onduleur "myups" situé sur la machine actuelle :

	upscmd -u admin -p adminpass myups@localhost test.battery.start