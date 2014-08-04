Importer la table ephemerides :

	mysql --user=root --password=mysql domotique < 	 /~serveur/bdd/backup_domotique/ephemerides.sql
	
**ATTENTION** Les ephemerides ont été calculées pour une maison en région toulousaine, si votre situation géographique est différente, il faut modifier la table.
*(nota : ajouter lien ou méthode de calcul)*