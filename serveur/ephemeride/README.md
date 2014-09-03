La table contient les heures de lever et coucher de soleil déterminés à partir de ce site : http://ptaff.ca/soleil/ (possibilité d'obtenir les infos au format CSV)

Importer la table ephemerides :

	mysql --user=root --password=mysql domotique < 	 /~serveur/bdd/backup_domotique/ephemerides.sql
	
**ATTENTION** Les ephemerides ont été calculées pour une maison en région toulousaine, si votre situation géographique est différente, il faut modifier la table.
*(nota : ajouter lien ou méthode de calcul)*
