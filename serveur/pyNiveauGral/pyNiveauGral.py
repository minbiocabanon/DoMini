#!/usr/bin/env python
import sys
import syslog
import time
import mariadb as mdb
from pushbullet import Pushbullet

# -- Global variables
ACCESS_TOKEN = 'o.lMLyP43FEWjOhJqVDN5NimjD0TyjC0UH'

date_niveaugral = 0
niveaugral = 0
NIVEAU_MINI = 33


#--- setup ---
def setup():
	print ('Setup')
	syslog.openlog("pyNiveauGral")
	syslog.syslog("Demarrage")
	print ('End Setup')
# -- fin setup --

# -- get all about time and date --
def get_time():
	print('\nget_time()')
	# display date/time
	logmessage = time.strftime(" Date et heure : %Y-%m-%d %H:%M:%S")
	print(logmessage)
	syslog.syslog(logmessage)
	# afficher le timestamp
	logmessage = " Unixtime : " + str(time.time())
	print(logmessage)
	syslog.syslog(logmessage)
# -- end get_time --

# -- recupere le derniver niveau de granules transmis par le poele --
def get_niveaugral():
	global date_niveaugral
	global niveaugral
	# requete pour lire la table contenant les logs de niveau de granules
	try:
		# Open MySQL session
		con = mdb.connect(user="root",password="mysql",host="localhost",database="domotique")
		cur = con.cursor()
		# prepare query
		query = 'SELECT * FROM  `pellets_rsv` ORDER BY date_time DESC LIMIT 0 , 1'
		# run MySQL Query
		cur.execute(query)
		req_niveaugral = cur.fetchone()
		rc = cur.rowcount
		# Close all cursors
		cur.close()
		# Close MySQL session
		con.close()
		# if no result, don't process result
		if (rc != 0) :
			# create variable
			date_niveaugral = req_niveaugral[1]
			niveaugral = req_niveaugral[3]
		else:
			date_niveaugral = str(time.time())
			niveaugral = 0
			
		
		# add some log
		logmessage = " date_niveaugral  : " +  str(date_niveaugral) + "\n niveaugral(cm) : " + str(niveaugral)
		print(logmessage)
		syslog.syslog(logmessage)
		
	except mdb.Error as e:
		# create variable
		date_niveaugral = 0
		niveaugral = 0
		# Display MySQL errors
		try:
			print("MySQL Error [%d]: %s" % (e.args[0], e.args[1]))
		except IndexError:
			print("MySQL Error: %s" % str(e))
		
# -- end get_niveaugral() --

# -- on verifie le niveau de granule, puis on envoie un message sur pushbullet si le niveau est trop bas --
def check_niveaugral():
	global date_niveaugral
	global niveaugral
	logmessage = " Niveau mini de granules : " + str(NIVEAU_MINI) + "cm \n"
	print(logmessage)
	syslog.syslog(logmessage)
	
	# requete pour lire la table contenant les logs de niveau de granules
	# attention : on mesure la profondeur du reservoir. la quantite de granule est donc la profondeur max du reservoir - la mesure (niveaugral)
	# on compare donc la profondeur a la profondeur MINI correspondant a un niveau de granule bas.
	if (niveaugral >= NIVEAU_MINI):
		# add some log
		logmessage = ' NIVEAU DE GRANULES TROP BAS ! ({0} cm)'.format(niveaugral)
		print(logmessage)
		syslog.syslog(logmessage)
		global ACCESS_TOKEN
		pb = Pushbullet(ACCESS_TOKEN)
		msg_pb = time.strftime("%Y-%m-%d %H:%M:%S") + "\n Attention, niveau de granules bas \n" + str(date_niveaugral) + " = " + str(niveaugral) + "cm"
		push = pb.push_note("Domini", msg_pb)		
	else :
		logmessage = " NIVEAU DE GRANULES OK."
		print(logmessage)
		syslog.syslog(logmessage)		
		
# -- end check_niveaugral() --


#--- obligatoire pour lancement du code --
if __name__=="__main__": # set executable code
	setup() 	# setup() function call
	get_time()
	get_niveaugral()
	check_niveaugral()
