### planning.c

#Synopsis
	planning [annee]

#Description
	planning permet de générer le planning pour domini avec une grille de 30 minutes. Ce calendrier est basé sur la table 'calendrier' de la BDD. Cette table doit être chargée manuellement avec un fichier CSV contenant tous les jours de l'année avec leur type de jour.

#Options
	[annee] : année pour laquelle il faut générer le calendrier. Par défaut, prend l'année du jour en cours.

#Versions
	01/01/2019 : ajout de la suppression des infos de l'année précédente dans la table 'calendrier' sinon le logiciel planning n'arrivait pas à affecter aux lignes de l'année en cours, les infos de temperatures.
