Periph
=====

Ce repertoire contient l'ensemble des sous-projets réalisés sur des modules Jeenodes (Arduino)

Lien utile : http://jeelabs.org/

##Compteur EDF
Module Jeenode relié à la sortie téléinfo du compteur EDF.
Transmet en continu les données de téléinfo (filtrées) au PC serveur.


##Congelo (pour test)
Module Jeenode m'ayant servi à surveillé la température de mon congélateur.
Module similaire au périphériques Sonde_temperature et station météo, mais n'envoie qu'une seule info (température de la CTN)


##Poele
Module Jeenode connecté au poele à granulés.
Il permet de gérer la puissance du poele selon les consignes envoyées par le PC serveur.
Un bouton permet d'indiquer qu'un ou plusieurs sacs de granulés a été vidés dans le réservoir et d'envoyer l'info au PC serveur (gestion du stock et consommation)


##Puits canadien
Voir 'sonde temperature' pour le code et les schémas. Ce répertoire ne contient que les feuilles de calculs et formules m'ayant servis à estimer la puissance récupérée par le puits canadien.
Module Jeenode équipé d'un capteur de température et d'humidité SHT11.
Installé dans le conduit du puits canadien, il envoie régulièrement les infos température, humidité, point de rosé et tension batterie.


##Pyranomètre (pour test)
Module Jeenode m'ayant servi à évaluer différentes solutions du pyranomètre (mesure du flux solaire)
installé dehors ou derrière une fenêtre, il envoie régulièrement les infos de flux solaire (bruts, la conversion est effectuée par le PC serveur)


##Robomow (pour test)
Module en cours de réalisation
Jeenode avec GPS , module radio RF12 et Sigfox -> Alarme et geotracking du robot tondeuse.


##sonde température
Module Jeenode équipé d'un capteur de température et d'humidité SHT11.
Il envoie régulièrement les infos température, humidité, point de rosé et tension batterie.


##Station météo
Module quasi identique à 'Sonde temperature'.
Le Jeenode est installé dans un boitier extérieur (type ruche).
Il mesure également le flux solaire (pyranomètre).
Les piles sont rechargées par un capteur solaire, le module est autonome en énergie.


##VMCDF_bypass
Module Jeenode qui pilote le bypass du puits canadien. Les consignes sont transmises par le PC serveur.
Le Jeenode mesure également la température ambiante (CTN) et la transmet au PC serveur.


##Volet roulants
Module Jeenode interfacé avec une télécommande SOMFY RTS.
Le Jeenode reçoit les consignes d'ouverture/fermeture des volets roulants. Il commute ensuite ses sorties pour piloter les canaux de la télécommande associés à chacune des volets roulants.
