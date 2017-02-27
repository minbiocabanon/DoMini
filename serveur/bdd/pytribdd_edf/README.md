pytribdd_edf.py
========

Script lancé quotidiennement par une action CRON

#utilisation sans arguments
	$python pytribdd_edf.py 
	
	ne converse que le premier et dernier relevés de téléinfo pour le jour J - NB_JOUR (constante à voir dans le code, 3 ou 15 jours)
	
#utilisation avec une date en arguments
	$python pytribdd_edf.py 2017-01-01
	
	ne conserve que le premier et dernier relevé de téléinfo pour chaque jour depuis la date passée en argument, et jusqu'à J - NB_JOUR.