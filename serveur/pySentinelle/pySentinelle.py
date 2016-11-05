#!/usr/bin/env python
import sys
import syslog
import time
import MySQLdb as mdb
from pushbullet import Pushbullet
import urllib
import os


#--- setup ---
def setup():
	print ('Setup')
	syslog.openlog("pySentinelle")
	syslog.syslog("Demarrage")
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

# -- test si le server web tourne --
def check_db():
	print '\ncheck_db()'
	# requete pour tester si la bdd tourne
	try:
		# Open MySQL session
		con = mdb.connect('localhost','root','mysql','domotique')
		# If connection fail, see except process hereafter
		#else 
		# Close MySQL session
		con.close()
		# add some log
		logmessage = " OK, Database is running"
		
	except mdb.Error, e:
		# Display MySQL errors
		try:
			print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
			logmessage = time.strftime("%Y-%m-%d %H:%M:%S") + " DoMini - Error : Database is NOT running"
			pb = Pushbullet('o.OVDjj6Pg0u8OZMKjBVH6QBqToFbhy1ug') 
			push = pb.push_note("Domini", logmessage)	
		except IndexError:
			print "MySQL Error: %s" % str(e)

	print logmessage
	syslog.syslog(logmessage)
			
# -- end check_db() --

def check_webserver():
	print '\ncheck_webserver()'
	code_ws = urllib.urlopen("http://192.168.0.102/index.php").getcode()
	
	if( code_ws != '200') :
		logmessage = " OK, server web is running"
	else :
		pb = Pushbullet('o.OVDjj6Pg0u8OZMKjBVH6QBqToFbhy1ug') 
		logmessage = time.strftime("%Y-%m-%d %H:%M:%S") + "\n Domini - Erreur : serveur web est arrete"
		push = pb.push_note("Domini", logmessage)	
	
	print logmessage
	syslog.syslog(logmessage)	
					
# -- end check_webserver() --

def check_receiver():
	print '\ncheck_receiver()'
	processname = 'pyreceiver.py'
	tmp = os.popen("ps -Af | grep '.py'").read()
	proccount = tmp.count(processname)

	if proccount > 0:
		logmessage = " OK, pyreceiver is running"
	else :
		pb = Pushbullet('o.OVDjj6Pg0u8OZMKjBVH6QBqToFbhy1ug') 
		logmessage = time.strftime("%Y-%m-%d %H:%M:%S") + "\n Domini - Erreur : pyreceiver est arrete"
		push = pb.push_note("Domini", logmessage)
		
	print logmessage
	syslog.syslog(logmessage)	
					
# -- end check_receiver() --

#--- obligatoire pour lancement du code --
if __name__=="__main__": # set executable code
	setup() 	# setup() function call
	get_time()
	check_webserver()
	check_db()
	check_receiver()
