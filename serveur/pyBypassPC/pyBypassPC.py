#!/usr/bin/env python
import sys
import syslog

import time
import MySQLdb as mdb
from time import sleep

# -- Global variables
ftemp_ext = 0
ftemp_pc = 0
inf_saison = 'A'
inf_type_jour = 'T'
ftemp_consigne = 20.0
consignebypass = 0

POSITION_PC = 0
POSITION_EXT = 100


#--- setup ---
def setup():
	print ('Setup')
	syslog.openlog("pyBypassPC")
	syslog.syslog("Demarrage")
	logmessage = " Demarrage pyBypassPC.py, logiciel de gestion du bypass du puits canadien \n 000 -> bypass donne l'air du puits canadien.\n 100 -> bypass donne l'air exterieur."
	print logmessage
	syslog.syslog(logmessage)
	print ('End Setup')
# -- fin setup --

# -- get all about time and date --
def get_time():
	print '\nget_time()'
	# display date/time
	logmessage = time.strftime(" Date et heure : %Y-%m-%d %H:%M:%S")
	print logmessage
	syslog.syslog(logmessage)
	# afficher le timestamp
	logmessage = " Unixtime : " + str(time.time())
	print logmessage
	syslog.syslog(logmessage)
# -- end get_time --

# -- recuperation des temperatures necessaires a l'algo de determination de la position du PC --
def read_temperature():
	print '\n read_temperature()'
	global ftemp_ext
	global ftemp_pc

	# Get tempereture exterieure
	try:
		# Open MySQL session
		con = mdb.connect('localhost','root','mysql','domotique')
		cur = con.cursor()
		# prepare query
		query = 'SELECT AVG( ana1 ) AS TempMoy FROM  `analog1` WHERE date_time >= SUBTIME( NOW( ) ,  \'00:30:00\' )'
		# run MySQL Query
		cur.execute(query)
		result = cur.fetchone()
		# Close all cursors
		cur.close()
		# Close MySQL session
		con.close()
	except mdb.Error, e:
		# create variable
		result = 0
		# Display MySQL errors
		try:
			print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
		except IndexError:
			print "MySQL Error: %s" % str(e)

	try:
		#get temperature
		ftemp_ext = result[0]
		# add some log about temperature value
		logmessage = "  temperature exterieur : " +  str(ftemp_ext)		
		print logmessage
		syslog.syslog(logmessage)
	except:
		# if error, print error returned
		logmessage = " ERROR while parsing mysql request (get temperature exterieure) : "
		print logmessage
		syslog.syslog(logmessage)

	# Get tempereture du PC
	try:
		# Open MySQL session
		con = mdb.connect('localhost','root','mysql','domotique')
		cur = con.cursor()
		# prepare query
		query = 'SELECT AVG( ana1 ) AS TempMoy FROM  `analog3` WHERE date_time >= SUBTIME( NOW( ) ,  \'00:30:00\' )'
		# run MySQL Query
		cur.execute(query)
		result = cur.fetchone()
		# Close all cursors
		cur.close()
		# Close MySQL session
		con.close()
	except mdb.Error, e:
		# create variable
		result = 0
		# Display MySQL errors
		try:
			print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
		except IndexError:
			print "MySQL Error: %s" % str(e)

	try:
		#get temperature
		ftemp_pc = result[0]
		# add some log about temperature value
		logmessage = "  temperature du PC : " +  str(ftemp_pc)		
		print logmessage
		syslog.syslog(logmessage)
	except:
		# if error, print error returned
		logmessage = " ERROR while parsing mysql request (get temperature exterieure) : "
		print logmessage
		syslog.syslog(logmessage)
		
# -- end read_temperature --

# -- recuperation des donnees relatives au jour --
def read_info_jour():
	print '\n read_info_jour()'
	global inf_saison
	global inf_type_jour

	# Get saison et type jour
	try:
		# Open MySQL session
		con = mdb.connect('localhost','root','mysql','domotique')
		cur = con.cursor()
		# prepare query
		query = 'SELECT * FROM `calendrier` WHERE `date` = DATE_FORMAT( NOW() , \'%y-%m-%d\' )'
		# run MySQL Query
		cur.execute(query)
		result = cur.fetchone()
		# Close all cursors
		cur.close()
		# Close MySQL session
		con.close()
	except mdb.Error, e:
		# create variable
		result = 0
		# Display MySQL errors
		try:
			print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
		except IndexError:
			print "MySQL Error: %s" % str(e)

	try:
		#get saison et type jour
		inf_saison = result[2]
		inf_type_jour = result[3]
		# add some log about temperature value
		logmessage = "  saison : " +  str(inf_saison) + "\n  type jour : " + str(inf_type_jour)
		print logmessage
		syslog.syslog(logmessage)
	except:
		# if error, print error returned
		logmessage = " ERROR while parsing mysql request (get saison et type jour) : "
		print logmessage
		syslog.syslog(logmessage)

# -- end read_info_jour --
		
# -- recuperation de la consigne de temperature --
def read_consigne():
	print '\n read_consigne()'
	global ftemp_consigne
	global inf_saison
	
	# Get consigne de temperature en degre C a partir de la saison
	try:
		# Open MySQL session
		con = mdb.connect('localhost','root','mysql','domotique')
		cur = con.cursor()
		# prepare query
		query = 'SELECT * 	FROM `calendrier_saison` WHERE `type` = \'{0}\''.format(inf_saison)
		# run MySQL Query
		cur.execute(query)
		result = cur.fetchone()
		# Close all cursors
		cur.close()
		# Close MySQL session
		con.close()
	except mdb.Error, e:
		# create variable
		result = 0
		# Display MySQL errors
		try:
			print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
		except IndexError:
			print "MySQL Error: %s" % str(e)

	try:
		#get consigne de temperature
		ftemp_consigne = result[2]
		# add some log about temperature value
		logmessage = "  consigne : " +  str(ftemp_consigne)
		print logmessage
		syslog.syslog(logmessage)
	except:
		# if error, print error returned
		logmessage = " ERROR while parsing mysql request (get consigne) : "
		print logmessage
		syslog.syslog(logmessage)
# -- end read_consigne --

# -- calcul de la consigne a appliquer au bypass --
def calcul_consigne():
	print '\n calcul_consigne()'
	global ftemp_pc
	global ftemp_ext
	global inf_saison
	global ftemp_consigne
	global consignebypass
	
	# Get consigne de temperature en degre C a partir de la saison
	if (inf_saison == "H") :
		#on cherche a faire entrer dans la VMC la temperature la plus chaude (no limit)
		if(ftemp_pc > ftemp_ext) :
			consignebypass = POSITION_PC
		else :
			consignebypass = POSITION_EXT
				
	elif ( inf_saison == "E" ) :
		#on cherche a faire entrer dans la VMC la temperature la plus froide (no limit)
		if(ftemp_pc > ftemp_ext) :
			consignebypass = POSITION_EXT
		else :
			consignebypass = POSITION_PC
			
	elif ( inf_saison == "A" ) :
		#on cherche a faire entrer dans la VMC la temperature la plus proche de la temperature du consigne
		if( abs(ftemp_pc - ftemp_consigne) > abs(ftemp_ext - ftemp_consigne)) :
			consignebypass = POSITION_EXT
		else :
			consignebypass = POSITION_PC
			
	else :
		try :
			# si y'a erreur on met la position par defaut
			consignebypass = POSITION_PC
			logmessage = " WARNING : Unknown saison (H or A or E): " + str(inf_saison) + "  Position par defaut : " + srt(consigne)
			print logmessage
			syslog.syslog(logmessage)
		except :
			logmessage = " ERROR while parsing unknown saison "
			print logmessage
			syslog.syslog(logmessage)

	logmessage = "  Position du bypass calculee : " + str(consignebypass)
	print logmessage
	syslog.syslog(logmessage)		
# -- end calcul_consigne --


# -- envoie la consigne bypass --
def send_consigne():
	global consignebypass

	print '\n send_consigne()'	
	# --- Add message into 'tx_msg_radio' table
	try:
		# Open MySQL session
		con = mdb.connect('localhost','root','mysql','domotique')
		cur = con.cursor()
		# prepare query
		query = 'INSERT INTO `domotique`.`tx_msg_radio` (`id` ,`date_time` ,`message` )VALUES (NULL , NOW(), \'$BPC,{0},0\');'.format(consignebypass)
		# run MySQL Query
		cur.execute(query)
		# Close all cursors
		cur.close()
		# Close MySQL session
		con.close()
		# log
		logmessage = "  Message envoye au bypass : $BPC,{0},0".format(consignebypass)
		print logmessage
		syslog.syslog(logmessage)
	except mdb.Error, e:
		# Display MySQL errors
		try:
			print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
		except IndexError:
			print "MySQL Error: %s" % str(e)	
# -- end send_etat_vr --


# -- Fonction qui log la structure stDonnees dans la BDD pour mise au point --
def log_donnees():
	global consignebypass

	print '\n send_consigne()'	
	# --- Add message into 'tx_msg_radio' table
	try:
		# Open MySQL session
		con = mdb.connect('localhost','root','mysql','domotique')
		cur = con.cursor()
		# prepare query
		query = 'INSERT INTO `domotique`.`bypass_pc_log` (`id`, `date_time`, `consigne`) VALUES (NULL, NOW(), \'{0}\');'.format(consignebypass)
		# run MySQL Query
		cur.execute(query)
		# Close all cursors
		cur.close()
		# Close MySQL session
		con.close()
		# log
		logmessage = "  Message de log ecrit dans la bdd"
		print logmessage
		syslog.syslog(logmessage)
	except mdb.Error, e:
		# Display MySQL errors
		try:
			print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
		except IndexError:
			print "MySQL Error: %s" % str(e)	
# -- end send_etat_vr --

#--- obligatoire pour lancement du code --
if __name__=="__main__": # set executable code
	setup()
	get_time()
	read_temperature()
	read_info_jour()
	read_consigne()
	calcul_consigne()
	send_consigne()
	log_donnees()