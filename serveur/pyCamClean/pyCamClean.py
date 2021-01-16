#!/usr/bin/env python
import sys
import syslog
import time
import os

now = 0
workdir = ""

NB_JOUR = 1


#--- setup ---
def setup():
	global workdir
	print ('Setup')
	syslog.openlog("pySentinelle")
	syslog.syslog("Demarrage")
	
	if len(sys.argv) != 2:
		print("usage", sys.argv[0], " <dir>")
		sys.exit(1)

	workdir = sys.argv[1]
	
	
	print ('End Setup')
# -- fin setup --

# -- get all about time and date --
def get_time():
	global now
	global workdir
	
	print('\nget_time()')
	# display date/time
	now = time.time()
	logmessage = time.strftime(" Date et heure : %Y-%m-%d %H:%M:%S")
	print(logmessage)
	syslog.syslog(logmessage)
# -- end get_time --

# -- suppression des vieux fichiers --
def remove_old_files():
	global now
	nbfile = 0
	print('\nremove_old_files()')
	# requete pour tester si la bdd tourne
	try:
		old = now - ( NB_JOUR * 24 * 60 * 60 )

		for f in os.listdir(workdir):
			path = os.path.join(workdir, f)
			if os.path.isfile(path):
				stat = os.stat(path)
				if stat.st_ctime < old:
					print("removing: ", path)
					nbfile = nbfile + 1
					os.remove(path) # uncomment when you will sure :)
		
		logmessage = " file(s) removed = " + str(nbfile)
		
	except :
		logmessage = "Error while deleting files"

	print(logmessage)
	syslog.syslog(logmessage)
			
# -- end remove_old_files() --

#--- obligatoire pour lancement du code --
if __name__=="__main__": # set executable code
	setup() 	# setup() function call
	get_time()
	remove_old_files()
