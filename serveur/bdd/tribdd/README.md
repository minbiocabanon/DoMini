pytribdd.py
========
Script lanc� quotidiennement par une action CRON

#utilisation sans arguments
	$python pytribdd_edf.py 
	
	ne converse que les releves de debut de chaque heure pour  le jour J - NB_JOUR (NB_JOUR en dur dans le code)
	
#utilisation avec une date en arguments
	$python pytribdd.py 2017-01-01
	
	ne converse que les releves de debut de chaque heure pour  l'intervalle de temps : "date en argument" jusqu'� "jour actuel - NB_JOUR" (NB_JOUR en dur dans le code)

========
A faire avant de compiler : 
 - Renomer passwd_.h en passwd.h
 - Compl�ter les informations pour la connexion � MySQL :
	- user -> nom de l'utilisateur de la base
    - password -> mot de passe de l'utilisateur 
    - database = "domotique"; ("domotique" est le nom de la base par d�faut, � modifier si vous avez changer le nom)
	- server = "localhost"; (ne pas modifier sauf si vous savez de ce que vous faite)
    