#!/usr/bin/env python
import sys
import syslog
import time
import mariadb as mdb
from pushbullet import Pushbullet
import urllib.request, urllib.parse, urllib.error
import os
import subprocess


ACCESS_TOKEN = 'o.lMLyP43FEWjOhJqVDN5NimjD0TyjC0UH'

#--- setup ---
def setup():
	print ('Setup')
	syslog.openlog("pySentinelle")
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
# -- end get_time --

# -- test si le server web tourne --
def check_db():
	print('\ncheck_db()')
	# requete pour tester si la bdd tourne
	try:
		# Open mariadb session
		con = mdb.connect(user="root",password="mysql",host="localhost",database="domotique")
		# If connection fail, see except process hereafter
		#else 
		# Close mariadb session
		con.close()
		# add some log
		logmessage = " OK, Database is running"
		
	except mdb.Error as e:
		# Display mariadb errors
		try:
			#print("mariadb Error [%d]: %s" % (e.args[0], e.args[1]))
			logmessage = time.strftime("%Y-%m-%d %H:%M:%S") + " DoMini - Error : Database is NOT running"
			#global ACCESS_TOKEN
			pb = Pushbullet(ACCESS_TOKEN)
			push = pb.push_note("Domini", logmessage)
			
			#restart mariadb
			print('\nrestart mariadb manually')
			os.system("sudo systemctl restart mariadb")
						
		except IndexError:
			logmessage = ""
			print("mariadb Error + pushbullet Error : %s" % str(e))

	print(logmessage)
	syslog.syslog(logmessage)
			
# -- end check_db() --

def check_webserver():
	print('\ncheck_webserver()')
	try:
		code_ws = urllib.request.urlopen("http://localhost/index.php").getcode()
		
		if( code_ws != '200') :
			logmessage = " OK, server web is running"
		else :
			global ACCESS_TOKEN
			pb = Pushbullet(ACCESS_TOKEN)
			logmessage = time.strftime("%Y-%m-%d %H:%M:%S") + "\n Domini - Erreur : serveur web est arrete"
			push = pb.push_note("Domini", logmessage)	
	except:
		logmessage = time.strftime("%Y-%m-%d %H:%M:%S") + "\n Domini - Erreur : etat serveur web inconnu"
		push = pb.push_note("Domini", logmessage)
		
	print(logmessage)
	syslog.syslog(logmessage)	
					
# -- end check_webserver() --

def check_receiver():
	print('\ncheck_receiver()')
	processname = 'pyreceiver.py'
	tmp = os.popen("ps -Af | grep '.py'").read()
	proccount = tmp.count(processname)

	if proccount > 0:
		logmessage = " OK, pyreceiver is running"
	else :
		global ACCESS_TOKEN
		pb = Pushbullet(ACCESS_TOKEN)
		logmessage = time.strftime("%Y-%m-%d %H:%M:%S") + "\n Domini - Erreur : pyreceiver est arrete"
		push = pb.push_note("Domini", logmessage)
		
		#running pyreceiver 
		print('\nrunning pyreceiver manually')
		args = ['python3','/home/jcaulier/src/domini/serveur/pyReceiver/pyreceiver.py']
		subprocess.Popen(args,shell=False,stdout=subprocess.PIPE)
		
		#os.system("python3 /home/jcaulier/src/domini/serveur/pyReceiver/pyreceiver.py &")
		
	print(logmessage)
	syslog.syslog(logmessage)	
					
# -- end check_receiver() --

def check_ups():
	print('\ncheck_ups()')
	processname = 'ups_daemonized.py'
	tmp = os.popen("ps -Af | grep '.py'").read()
	proccount = tmp.count(processname)

	if proccount > 0:
		logmessage = " OK, ups_daemonized is running"
	else :
		global ACCESS_TOKEN
		pb = Pushbullet(ACCESS_TOKEN)
		logmessage = time.strftime("%Y-%m-%d %H:%M:%S") + "\n Domini - Erreur : ups_daemonized est arrete"
		push = pb.push_note("Domini", logmessage)
		
	print(logmessage)
	syslog.syslog(logmessage)	
					
# -- end check_receiver() --

#--- obligatoire pour lancement du code --
if __name__=="__main__": # set executable code
	setup() 	# setup() function call
	get_time()
	check_webserver()
	check_db()
	check_receiver()
	#check_ups()
