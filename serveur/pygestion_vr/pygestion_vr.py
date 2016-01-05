#!/usr/bin/env python
import sys
import syslog

import time
import MySQLdb as mdb
from time import sleep

# -- Global variables
vr_mode = 0
etat_vr_bureau = 0
etat_vr_salon = 0 
etat_vr_chm = 0
etat_vr_chjf = 0

# --- function for enumerate
def enum(*args):
	enums = dict(zip(args, range(len(args))))
	return type('Enum', (), enums)
# -- end enum --

VOLET = enum('FERME', 'OUVERT', 'MIOMBRE', 'IMMOBILE')
MODE_VR = enum('AUTOMATIQUE', 'TOUS_FERME', 'TOUS_OUVERT', 'MANUEL')

#--- setup ---
def setup():
	print ('Setup')
	syslog.openlog("pyGestion_vr")
	syslog.syslog("Demarrage")
	logmessage = " Infos utiles :\n 0 -> VOLET_FERME , 1 -> VOLET_FERME , 2 -> VOLET_MIOMBRE , 3 -> IMMOBILE"
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

# -- recuperation de l'etat des volets roulants --
def read_etat_vr():
	print '\nread_etat_vr()'
	global vr_mode
	global etat_vr_bureau
	global etat_vr_salon
	global etat_vr_chm
	global etat_vr_chjf

	# Get shutters's status
	try:
		# Open MySQL session
		con = mdb.connect('localhost','root','mysql','domotique')
		cur = con.cursor()
		# prepare query
		query = 'SELECT * FROM `voletroulant_statut`'
		# run MySQL Query
		cur.execute(query)
		status_shutters = cur.fetchone()
		# Close all cursors
		cur.close()
		# Close MySQL session
		con.close()
	except mdb.Error, e:
		# create variable
		status_shutters = 0
		# Display MySQL errors
		try:
			print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
		except IndexError:
			print "MySQL Error: %s" % str(e)

	try:
		#get mode
		vr_mode = status_shutters[2]
		#get status for all shutters
		etat_vr_bureau = status_shutters[3]
		etat_vr_salon = status_shutters[4]
		etat_vr_chm = status_shutters[5]
		etat_vr_chjf = status_shutters[6]
		# add some log about shutters status
		logmessage = " mode  : " +  str(vr_mode) + "\n etat_vr_bureau : " + str(etat_vr_bureau)+ "\n etat_vr_salon : " + str(etat_vr_salon)+ "\n etat_vr_chm : " + str(etat_vr_chm)+ "\n etat_vr_chjf : " + str(etat_vr_chjf)
		print logmessage
		syslog.syslog(logmessage)
	except:
		# if error, print error returned
		logmessage = " ERROR while parsing mysql request (mode and vr status) : " + status_shutters
		print logmessage
		syslog.syslog(logmessage)
		
# -- end read_etat_vr --

# -- determine l'etat des volets roulants --
def manage_vr():
	global vr_mode
	global MODE_VR
	global VOLET
	global etat_vr_bureau
	global etat_vr_salon
	global etat_vr_chm
	global etat_vr_chjf	

	print '\nmanage_vr()'
	#print '\n MODE_VR.AUTOMATIQUE : ' + str(MODE_VR.AUTOMATIQUE)
	#print '\n MODE_VR.MANUEL : ' + str(MODE_VR.MANUEL)
	#print '\n vr_mode : ' + str(vr_mode)
	
	if str(vr_mode) == str(MODE_VR.AUTOMATIQUE) :
#	if str(vr_mode) == str(MODE_VR.MANUEL) :
		# --- get ephemeris from db
		print ' Get ephemeris data'
		try:
			# Open MySQL session
			con = mdb.connect('localhost','root','mysql','domotique')
			cur = con.cursor()
			# prepare query
			query = 'SELECT UNIX_TIMESTAMP(addtime(DATE_FORMAT(MAKEDATE(YEAR(NOW()),DAYOFYEAR(NOW())),\'%Y-%m-%d %H:%i:%s\') , `lever`)) AS hlever_unix, UNIX_TIMESTAMP(addtime(DATE_FORMAT(MAKEDATE(YEAR(NOW()),DAYOFYEAR(NOW())),\'%Y-%m-%d %H:%i:%s\') , `coucher`)) AS hcoucher_unix FROM `ephemerides` WHERE DATE_FORMAT( `mois-jour` , \'%m-%d\' ) = DATE_FORMAT( NOW( ) , \'%m-%d\' ) LIMIT 1'
			# run MySQL Query
			cur.execute(query)
			result = cur.fetchone()
			# Close all cursors
			cur.close()
			# Close MySQL session
			con.close()
		except mdb.Error, e:
			# Display MySQL errors
			try:
				print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
			except IndexError:
				print "MySQL Error: %s" % str(e)
		try:
			h_lever = result[0]
			h_coucher = result[1]

			logmessage = "  h_lever  : " +  str(h_lever) + "\n  h_coucher : " + str(h_coucher)
			print logmessage
			syslog.syslog(logmessage)
		except:
			# if error, print error returned
			logmessage = " ERROR while parsing mysql request (ephemeris) : " + result
			print logmessage
			syslog.syslog(logmessage)
		
		# --- Is it nigh or day ?
		logmessage = " Determine if it is night(0) or day(1)"
		print logmessage
		syslog.syslog(logmessage)
		
		if time.time() >= h_lever and time.time() < h_coucher:
			flag_jour = 1
		else :
			flag_jour = 0

		logmessage = "  flag_jour  : " +  str(flag_jour)
		print logmessage
		syslog.syslog(logmessage)
			
		# --- get info about current day in db table
		print ' Get info about current day'
		try:
			# Open MySQL session
			con = mdb.connect('localhost','root','mysql','domotique')
			cur = con.cursor()
			# prepare query
			query = 'SELECT * FROM `calendrier` WHERE `date` >= DATE_FORMAT( NOW( ) , \'%y-%m-%d\' ) LIMIT 0,1'
			# run MySQL Query
			cur.execute(query)
			result = cur.fetchone()
			# Close all cursors
			cur.close()
			# Close MySQL session
			con.close()
		except mdb.Error, e:
			# Display MySQL errors
			try:
				print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
			except IndexError:
				print "MySQL Error: %s" % str(e)
		try:
			saison = result[2]
			type_jour = result[3]
			logmessage = "  saison  : " +  str(saison) + "\n  type_jour : " + str(type_jour)
			print logmessage
			syslog.syslog(logmessage)
		except:
			# if error, print error returned
			logmessage = " ERROR while parsing mysql request (calendrier) : " + result
			print logmessage
			syslog.syslog(logmessage)

		# --- get info about solar flux
		print ' Get solar flux'
		try:
			# Open MySQL session
			con = mdb.connect('localhost','root','mysql','domotique')
			cur = con.cursor()
			# prepare query
			query = 'SELECT AVG(ana1)*454 FROM `pyranometre` WHERE date_time >= SUBTIME( now( ) , \"00:30:00.0\" ) '
			# run MySQL Query
			cur.execute(query)
			result = cur.fetchone()
			# Close all cursors
			cur.close()
			# Close MySQL session
			con.close()
		except mdb.Error, e:
			# set value to 0
			fPyraMoy = 0.0
			# Display MySQL errors
			try:
				print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
			except IndexError:
				print "MySQL Error: %s" % str(e)
		try:
			fPyraMoy = result[0]
			logmessage = "  Solar flux mean value : " +  str(fPyraMoy)
			print logmessage
			syslog.syslog(logmessage)
		except:
			# if error, print error returned
			logmessage = " ERROR while parsing mysql request (pyranometre) : " + result
			print logmessage
			syslog.syslog(logmessage)
			fPyraMoy = 0.0

		# --- get temperature to asserve and solar flux trig
		print ' Get temperature to asserve and solar flux trig'
		try:
			# Open MySQL session
			con = mdb.connect('localhost','root','mysql','domotique')
			cur = con.cursor()
			# prepare query
			query = 'SELECT `consigne_temperature`, `flux solaire` FROM calendrier_saison WHERE type = "{0}"'.format(saison)
			# run MySQL Query
			cur.execute(query)
			result = cur.fetchone()
			# Close all cursors
			cur.close()
			# Close MySQL session
			con.close()
		except mdb.Error, e:
			# set value to 0
			consigne_temperat = 0.0
			# Display MySQL errors
			try:
				print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
			except IndexError:
				print "MySQL Error: %s" % str(e)
		try:
			consigne_temperat = result[0]
			flux_solaire_trig = result[1]
			logmessage = "  consigne_temperat : " +  str(consigne_temperat) + "\n  flux_solaire_trig : " + str(flux_solaire_trig)
			print logmessage
			syslog.syslog(logmessage)
		except:
			# if error, print error returned
			logmessage = " ERROR while parsing mysql request (calendrier_saison) : " + result
			print logmessage
			syslog.syslog(logmessage)
			consigne_temperat = 0.0
			
		# --- get interior temperature
		print ' Get interior temperature'
		try:
			# Open MySQL session
			con = mdb.connect('localhost','root','mysql','domotique')
			cur = con.cursor()
			# prepare query
			query = 'SELECT ana1, date_time FROM analog2 WHERE date_time >= SUBTIME( NOW( ) , \'00:15:00\' ) ORDER BY date_time LIMIT 0 , 1'
			# run MySQL Query
			cur.execute(query)
			result = cur.fetchone()
			# Close all cursors
			cur.close()
			# Close MySQL session
			con.close()
		except mdb.Error, e:
			# set value to 0
			temp_int = 0.0
			# Display MySQL errors
			try:
				print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
			except IndexError:
				print "MySQL Error: %s" % str(e)
		try:
			temp_int = result[0]
			logmessage = "  Temperature interieure : " +  str(temp_int)
			print logmessage
			syslog.syslog(logmessage)
			#on regarde si la temperature de consigne est atteinte (depassee) ou non
			if temp_int > consigne_temperat :
				flag_temperature = 1
			else :
				flag_temperature = 0
				
		except:
			# if error, print error returned
			logmessage = " ERROR while parsing mysql request (analog2) : " + result
			print logmessage
			syslog.syslog(logmessage)
			temp_int = 0.0
			
		# on positionne les flags selon la saison et le flux solaire seuil
		# si on a calcule un flux solaire coherent
		if fPyraMoy > 0.0 :
			# si on est en hiver ou en inter saison
			if saison == "H" or saison == "A" :
				# on regarde si le flux actuel est > au mini demande en hiver
				if fPyraMoy > flux_solaire_trig :
					flag_flux = 1
				else :
					flag_flux = 0
			# si on est en ETE
			else :
				#on regarde si le flux actuel est < au maxi demande en ete
				if flux_solaire < flux_solaire_trig :
					flag_flux = 1
				else :
					flag_flux = 0
		else :
			# on force l'etat en 'immobile'
			etat_vr_bureau = etat_vr_chjf = etat_vr_chm = etat_vr_salon = 3
			logmessage = " Etat des VR force a immobile (3) car fPyraMoy = " + fPyraMoy
			print logmessage
			syslog.syslog(logmessage)

		# --- on passe les conditions dans la table de verite afin de recuperer l'etat des VR
		print ' compute VR status'
		try:
			# Open MySQL session
			con = mdb.connect('localhost','root','mysql','domotique')
			cur = con.cursor()
			# prepare query
			query = 'SELECT `VOLET BUREAU`,`VOLET SALON`,`VOLET CHM`,`VOLET CHJF` FROM `voletroulant_table_verite` WHERE `saison`= "{0}" AND `flux solaire` = {1} AND `jour/nuit`= {2} AND `Temperature consigne`={3}'.format(saison, flag_flux, flag_jour, flag_temperature)
			# run MySQL Query
			cur.execute(query)
			vr_states = cur.fetchone()
			# Close all cursors
			cur.close()
			# Close MySQL session
			con.close()
		except mdb.Error, e:
			# set value to 0
			temp_int = 0.0
			# Display MySQL errors
			try:
				print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
			except IndexError:
				print "MySQL Error: %s" % str(e)
		try:
			# for result, extract status of each vr
			etat_vr_bureau = str(vr_states[0])
			etat_vr_salon = str(vr_states[1])
			etat_vr_chm = str(vr_states[2])
			etat_vr_chjf = str(vr_states[3])
			logmessage = "  etat_vr_bureau : {0},  etat_vr_salon : {1}, etat_vr_chm : {2}, etat_vr_chjf : {3}".format(etat_vr_bureau, etat_vr_salon, etat_vr_chm, etat_vr_chjf)
			print logmessage
			syslog.syslog(logmessage)
				
		except:
			# if error, print error returned
			logmessage = " ERROR while parsing mysql request (voletroulant_table_verite) : " + vr_states
			print logmessage
			syslog.syslog(logmessage)

		# --- on sauvegarde les etats dans la base de donnees
		# --- Save vr status in data base
		print ' compute VR status'
		try:
			# Open MySQL session
			con = mdb.connect('localhost','root','mysql','domotique')
			cur = con.cursor()
			# prepare query
			query = 'UPDATE `domotique`.`voletroulant_statut` SET `bureau` = \'{0}\',`salon` = \'{1}\',`chambreM` = \'{2}\',`chambreJF` = \'{3}\' WHERE `voletroulant_statut`.`id` =1;'.format(etat_vr_bureau, etat_vr_salon, etat_vr_chm, etat_vr_chjf)
			# run MySQL Query
			cur.execute(query)
			# Close all cursors
			cur.close()
			# Close MySQL session
			con.close()
			# log
			logmessage = "  Donnees stockees dans la table : $VRL,{0},{1},{2},{3}".format(etat_vr_bureau, etat_vr_salon, etat_vr_chm, etat_vr_chjf)
			print logmessage
			syslog.syslog(logmessage)
		except mdb.Error, e:
			# Display MySQL errors
			try:
				print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
			except IndexError:
				print "MySQL Error: %s" % str(e)
		
		# add log message about VR
		try:
			# Open MySQL session
			con = mdb.connect('localhost','root','mysql','domotique')
			cur = con.cursor()
			# prepare query
			query = 'INSERT INTO `domotique`.`voletroulant_log` (`id`, `date_time`, `bureau`, `salon`, `chambreM`, `chambreJF`) VALUES (NULL, NOW(), \'{0}\', \'{1}\', \'{2}\', \'{3}\');'.format(etat_vr_bureau, etat_vr_salon, etat_vr_chm, etat_vr_chjf)
			# run MySQL Query
			cur.execute(query)
			# Close all cursors
			cur.close()
			# Close MySQL session
			con.close()
			# log
			logmessage = "  Add log message in voletroulant_log"
			print logmessage
			syslog.syslog(logmessage)
		except mdb.Error, e:
			# Display MySQL errors
			try:
				print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
			except IndexError:
				print "MySQL Error: %s" % str(e)

				
# -- end manage_vr --

# -- envoie l'etat des vr par radio --
def send_etat_vr():
	global etat_vr_bureau
	global etat_vr_salon
	global etat_vr_chm
	global etat_vr_chjf	
	print '\nsend_etat_vr()'
	
	# --- Add message into 'tx_msg_radio' table
	try:
		# Open MySQL session
		con = mdb.connect('localhost','root','mysql','domotique')
		cur = con.cursor()
		# prepare query
		query = 'INSERT INTO `domotique`.`tx_msg_radio` (`id` ,`date_time` ,`message` )VALUES (NULL , NOW(), \'$VRL,{0},{1},{2},{3}\');'.format(etat_vr_bureau, etat_vr_salon, etat_vr_chm, etat_vr_chjf)
		# run MySQL Query
		cur.execute(query)
		# Close all cursors
		cur.close()
		# Close MySQL session
		con.close()
		# log
		logmessage = "  Message envoye au volet : $VRL,{0},{1},{2},{3}".format(etat_vr_bureau, etat_vr_salon, etat_vr_chm, etat_vr_chjf)
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
	read_etat_vr()
	manage_vr()
	send_etat_vr()