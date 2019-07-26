#!/usr/bin/env python
import sys
import syslog
import time
import os
import subprocess
from pushbullet import Pushbullet
import MySQLdb as mdb
from threading import Thread

import daemon

# -- USER variables
PERIOD_BDD = 1				# in seconds, delay between two database request
TIMEOUT_CAPTURE = 10		# in seconds, how long we take capture
PERIOD_CAPTURE = 1			# in seconds, delay between to camera capture
TIMEOUT_NOTIF = 60*5		# in secondes, delay between two pushbullet notification (to avoid flood)
# -- Global variables
tabname = "alarme_pir"
ACCESS_TOKEN = 'o.lMLyP43FEWjOhJqVDN5NimjD0TyjC0UH'
SM_state = "SM_ALARMOFF"
timeoutbdd = 0
timeoutcapture = 0
timeoutnotification = 0

# ipcam
IP_garage = "192.168.0.119:554"
IP_jardin = "192.168.0.120:554"
IP_porche = "192.168.0.129:554"
IP_sejour = "192.168.0.130:88"

class capturecam(Thread):
	#Thread charge de faire des captures d'images d'une webcam
	def __init__(self, ipcam):
		Thread.__init__(self)
		self.ipcam = ipcam

	def run(self):
	#Code a executer pendant l'execution du thread.

		global timeoutcapture
		#set flagCaptureDone to false
		flagCaptureDone = False
		
		#infinite loop
		while True :
			# update time and timeout , note that timeoutcapture is a global variable that can be modified in statemachine (when PIR sensor detection occurs)
			now = int(time.time())
			# loop capture intil timeout
			if now < timeoutcapture :
			
				# each can IP cam can be different, so command are specific
				if self.ipcam == IP_garage :
					dir = "../www/domini/webcam/garage/"+ time.strftime('%Y%m%d%H%M%S') +"_garage.jpg"
					commandargs = [
						'/usr/local/bin/ffmpeg',
						'-y',
						'-rtsp_transport','tcp',
						'-i', "rtsp://"+ str(self.ipcam) +"/live0.264",
						'-f', 'singlejpeg',
						'-vframes', '1',
						str(dir)
					]
					
				elif  str(self.ipcam) == IP_jardin:
					dir = "../www/domini/webcam/jardin/"+ time.strftime('%Y%m%d%H%M%S') +"_jardin.jpg"
					commandargs = [
						'/usr/local/bin/ffmpeg',
						'-y',
						'-rtsp_transport','tcp',
						'-i', "rtsp://"+ str(self.ipcam) +"/live0.264",
						'-f', 'singlejpeg',
						'-vframes', '1',
						str(dir)
					]
					
				elif  str(self.ipcam) == IP_porche:
					dir = "../www/domini/webcam/porche/"+ time.strftime('%Y%m%d%H%M%S') +"_porche.jpg"
					commandargs = [
						'/usr/local/bin/ffmpeg',
						'-y',
						'-rtsp_transport','tcp',
						'-i', "rtsp://"+ str(self.ipcam) +"/live0.264",
						'-f', 'singlejpeg',
						'-vframes', '1',
						str(dir)
					]

				elif str(self.ipcam) == IP_sejour :
					dir = "../www/domini/webcam/sejour/"+ time.strftime('%Y%m%d%H%M%S') +"_sejour.jpg"
					commandargs = [
						'curl',
						'--silent',
						'--url',"http://"+ str(self.ipcam) +"/cgi-bin/CGIProxy.fcgi?cmd=snapPicture2&usr=user&pwd=userFoscamC2",
						'--output', str(dir)
					]
				
				print "  [thread]Capture "+str(self.ipcam)
				FNULL = open(os.devnull, 'w')	
				subprocess.call(commandargs, stdout=FNULL, stderr=subprocess.STDOUT)
				#subprocess.call(commandargs, stdout=None, stderr=subprocess.STDOUT)
				time.sleep(PERIOD_CAPTURE)
				now = int(time.time())
				flagCaptureDone = True
			
			elif flagCaptureDone == True :
				print "  [thread]Fin capture "+str(self.ipcam)
				flagCaptureDone = False
			
			time.sleep(0.1)
			
# -- fin class capturecam --
		
		
# thread
capturecam_sejour = capturecam(IP_sejour)
capturecam_garage = capturecam(IP_garage)
capturecam_porche = capturecam(IP_porche)
capturecam_jardin = capturecam(IP_jardin)
		
#--- setup ---
def setup():
	global timeoutbdd
	print ('Setup')
	syslog.openlog("pyCaptureCam")
	syslog.syslog("Demarrage")
	timeoutbdd = int(time.time())
	
	print ('Lancement des threads capturecam')
	global capturecam_sejour
	global capturecam_garage
	global capturecam_porche
	global capturecam_jardin
	capturecam_sejour.start()
	capturecam_garage.start()
	capturecam_porche.start()
	capturecam_jardin.start()
	
	print ('End Setup')
# -- fin setup --

# -- get all about time and date --
def get_time():
	print '\nget_time()'
	# display date/time
	logmessage = time.strftime(" Date et heure : %Y-%m-%d %H:%M:%S")
	print logmessage
	syslog.syslog(logmessage)
# -- end get_time --


# -- tag_line --
def tag_line(table, id):
	try :
		# Open MySQL session
		con = mdb.connect('localhost','root','mysql','domotique')
		cur = con.cursor()
		# prepare query
		query = 'UPDATE `domotique`.`{0}` SET `tag` = \'1\' WHERE `{1}`.`id` ={2};'.format(table, table,id)
		print query
		# run MySQL Query
		cur.execute(query)
		# Close all cursors
		cur.close()
		#commit change
		con.commit()
		# Close MySQL session
		con.close()
		logmessage = "  Ligne "+ str(id) +" taggee"
		print logmessage
	except mdb.Error, e:
		# Display MySQL errors
		try:
			print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
		except IndexError:
			print "MySQL Error: %s" % str(e)
# -- fin tag_line --

# -- tag_line --
# -- when alarme is OFF, clear all PIR detection 
def tag_all_line(table):
	try :
		# Open MySQL session
		con = mdb.connect('localhost','root','mysql','domotique')
		cur = con.cursor()
		# prepare query to set all tag field to 1
		query = 'UPDATE `domotique`.`{0}` SET `tag` = 1 WHERE `tag` = 0;'.format(table) 
		#print query
		# run MySQL Query
		cur.execute(query)
		# Close all cursors
		cur.close()
		#commit change
		con.commit()
		# Close MySQL session
		con.close()
		logmessage = "  Toutes les Lignes ont ete taggees !"
		print logmessage
	except mdb.Error, e:
		# Display MySQL errors
		try:
			print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
		except IndexError:
			print "MySQL Error: %s" % str(e)
# -- fin tag_line --

# -- sendnotifpir --
#send message through pushbullet to user
def sendnotifpir():
	global timeoutnotification
	
	now = int(time.time())
	
	#check if a notification was send before recently
	if now > timeoutnotification :
		# launch timeoutnotification
		timeoutnotification = int(time.time()) + TIMEOUT_NOTIF
		# initialize pushbullet 
		pb = Pushbullet(ACCESS_TOKEN)
		logmessage = time.strftime("%Y-%m-%d %H:%M:%S") + "Detection PIR"
		push = pb.push_note("Domini - Alarme", logmessage)
		
		
# -- fin sendnotifpir --						
						

def Error_loop():
	# un peu bourrin : on boucle sur le message d'erreur + envoie d'info tant que le pb n'est pas resolu
	while True :
		logmessage = time.strftime("%Y-%m-%d %H:%M:%S") + "Alarme en mode ERROR !"
		print logmessage
		syslog.syslog(logmessage)
		# send message through pushbullet to user
		# initialize pushbullet 
		pb = Pushbullet(ACCESS_TOKEN)
		push = pb.push_note("Domini - Alarme", logmessage)
		time.sleep(600)

# -- statemachine --
def statemachine():
	print '\n --- statemachine() ---'
	
	global SM_state
	global ACCESS_TOKEN
	global timeoutbdd
	global timeoutcapture
	global capturecam_sejour
	global capturecam_garage
	global capturecam_porche
	global capturecam_jardin	
	
	# Select state of state machine

	# state SM_ALARMOFF
	if SM_state == "SM_ALARMOFF" :
		print "SM_ALARMOFF"
		# check if alarme is activated or not
		try:
			# Open MySQL session
			con = mdb.connect('localhost','root','mysql','domotique')
			cur = con.cursor()
			# prepare query
			query = 'SELECT `id`,`mode` FROM `alarme_statut`'
			# run MySQL Query
			cur.execute(query)
			result = cur.fetchone()
			nbrow = cur.rowcount
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
			#default value
			alarme_statut = "ERROR"

		try:
			# test if query has return something, if = 0 there is no line to tag so do nothing
			if (nbrow != 0):
				# get info from query result
				id = result[0]
				alarme_statut = result[1]
				logmessage = "  id: "+ str(id) +", alarme_statut: " +  str(alarme_statut)
				print logmessage
				#syslog.syslog(logmessage)
				
				if alarme_statut == "ON" :
					# go to SM_WAIT and wait for event
					SM_state = "SM_WAIT"					
				
				elif alarme_statut == "OFF" :
					# stay in SM_ALARMOFF
					SM_state = "SM_ALARMOFF"										
					# tag all PIR detection
					tag_all_line(tabname);
							
				elif alarme_statut == "ERROR" :
					Error_loop()
			else:
				Error_loop()
		except :
			logmessage = "  Erreur dans le traitement des donnees alarme_statut"	
			print logmessage


	# state SM_WAIT
	if SM_state == "SM_WAIT" :
		print "SM_WAIT"
		# check if timer for check PIR sensor status is expired
		now = int(time.time())
		if now > timeoutbdd :
			# timeoutbdd expired, time to make a request
			SM_state = "SM_GETPIR"
			# relaunch timeoutbdd
			timeoutbdd = int(time.time()) + int(PERIOD_BDD)

	# state SM_GETPIR
	if SM_state == "SM_GETPIR" :
		print "SM_GETPIR"
		# proceed to a db request to check if there is a PIR sensor alert
		try:
			# Open MySQL session
			con = mdb.connect('localhost','root','mysql','domotique')
			cur = con.cursor()
			# prepare query
			query = 'SELECT `id`,UNIX_TIMESTAMP(`date_time`),UNIX_TIMESTAMP(NOW()), `entete`,`etat` FROM `{0}` WHERE `tag`= 0 ORDER BY `date_time` DESC LIMIT 1'.format(tabname)
			# run MySQL Query
			cur.execute(query)
			result = cur.fetchone()
			nbrow = cur.rowcount
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
			#default value
			etat_PIR = 0
	
		# try:
			# test if query has return something, if = 0 there is no line to tag so do nothing
		if (nbrow != 0):
			# get info from query result
			id = result[0]
			datetime_pir = result[1]
			datetime_now = result[2]
			entete = result[3]
			etat_PIR = result[4]
			logmessage = "  id: "+ str(id) +", datetime_pir: " +  str(datetime_pir) + ", entete: " + str(entete) + ", etat_PIR: " + str(etat_PIR)
			print logmessage
			#syslog.syslog(logmessage)
			#logmessage = "  datetime_now - datetime_pir : "+ str(datetime_now - datetime_pir)
			#print logmessage
			# if alarm newer than 10 minutes
			if ( (datetime_now - datetime_pir) < 600 ) :
				# PIR detection has to be considered
				logmessage = "  datetime_now - datetime_pir : "+ str(datetime_now - datetime_pir)
				print logmessage
				if etat_PIR == 1 :
					# switch to SM_CAPTURE
					SM_state = "SM_CAPTURE"

					# prepare a message to send
					logmessage = time.strftime("%Y-%m-%d %H:%M:%S") + " detection presence PIR !!!!"
					# print message and log it
					print logmessage
					#syslog.syslog(logmessage)
					
					# send notification
					sendnotifpir()

					# tag the line and do nothing
					tag_line(tabname, id)
				else:
					SM_state = "SM_WAIT"
					# relaunch timeoutbdd
					timeoutbdd = int(time.time()) + PERIOD_BDD						
			else :
				# alarm older than 10 minutes are not considered
				# tag the line and do nothing
				tag_line(tabname, id)
				logmessage = "  Detection PIR > 600 secondes, pas de capture"
		else:
			logmessage = "  Pas de donnees a tagger"
			SM_state = "SM_ALARMOFF"
						
		print logmessage
		sys.stdout.flush()
			
		# except :
			# logmessage = "  Erreur dans le traitement des donnees etat_PIR"	
			# print logmessage			
		
		

			
	# state SM_CAPTURE
	elif SM_state == "SM_CAPTURE" :
		print "SM_CAPTURE"
		# update timeoutcapture , if thread are running they will continue to capture images
		timeoutcapture = int(time.time()) + TIMEOUT_CAPTURE
		print "    Capture en cours..."
		#force SM_state to SM_GETPIR
		SM_state = "SM_GETPIR"

# -- end statemachine() --

def Cortex():
	setup()
	get_time()
	while True:
		statemachine()
		time.sleep(1)

def run():
	with daemon.DaemonContext():
		Cortex()

if __name__ == "__main__":
	##run()
	try:
		Cortex()
		
	except KeyboardInterrupt:	#pour gerer le ctrl+c , interrupt handler
		print 'Interrupted by user'
		try:
			sys.exit(0)
		except SystemExit:
			os._exit(0)	