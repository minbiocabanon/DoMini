------ POUR TEST SEULEMENT -------

Ce répertoire n'a servi que pour le test de la mise en oeuvre du pyranomètre

    ~/periph/station_meteo

quelques explications sur mon blog : http://minbiocabanon.free.fr/index.php?tag/pyranometre et celui de bricolsec : http://bricolsec.canalblog.com/archives/2010/08/08/18770334.html

#Schéma 
2 montages de mises en oeuvre d'une BPW34 :


- Le premier montage semble bien adapté (avec polarisation directe de la BPW34):
j'ai des pics indésirables qui ressemblent à des erreurs d'acquisition de la mesure, l'amplitude est suffisamment grande pour détecter les périodes de soleil ou de couvertures. 
A voir s'il faut l'amplifier car en Juin le soleil cognera plus dur qu'en Février...

- Le deuxième montage (avec AOP) :
Trop sensible, il sature rapidement, je vais regarder à calmer un peu la sensibilité.


#sketches arduino
- lecture des deux valeurs analogiques
- conversion en volt
- transmission de la valeur en volt à la domotique (la conversion V-> w:/m² sera faite côté serveur)






