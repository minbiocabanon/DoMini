### planning.c

#Synopsis
	planning [annee]

#Description
	planning permet de g�n�rer le planning pour domini avec une grille de 30 minutes. Ce calendrier est bas� sur la table 'calendrier' de la BDD. Cette table doit �tre charg�e manuellement avec un fichier CSV contenant tous les jours de l'ann�e avec leur type de jour.

#Options
	[annee] : ann�e pour laquelle il faut g�n�rer le calendrier. Par d�faut, prend l'ann�e du jour en cours.

#Versions
	01/01/2019 : ajout de la suppression des infos de l'ann�e pr�c�dente dans la table 'calendrier' sinon le logiciel planning n'arrivait pas � affecter aux lignes de l'ann�e en cours, les infos de temperatures.
