#!/usr/bin/env python
import sys
import syslog
import time
import mariadb as mdb
from pushbullet import Pushbullet
import os
#import PyNUT
from nut2 import PyNUTClient
import json

# -- USER variables
PERIOD_MSG = 10		# in minutes, delay two messages with status info while power outtage

# -- Global variables
ups_status = ""
battery_charge = 0 
battery_runtime = 800
output_voltage = 230
SM_state = "POWERON"
timeout = 0


#--- setup ---
def setup():
	print ('Setup')
	syslog.openlog("pyUps")
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
		# Open MySQL session
		con = mdb.connect(user="root",password="mysql",host="localhost",database="domotique")
		# If connection fail, see except process hereafter
		#else 
		# Close MySQL session
		con.close()
		# add some log
		logmessage = " OK, Database is running"
		
	except mdb.Error as e:
		# Display MySQL errors
		try:
			print("MySQL Error [%d]: %s" % (e.args[0], e.args[1]))
			logmessage = time.strftime("%Y-%m-%d %H:%M:%S") + " DoMini - Error : Database is NOT running"
			pb = Pushbullet('o.OVDjj6Pg0u8OZMKjBVH6QBqToFbhy1ug') 
			push = pb.push_note("Domini", logmessage)	
		except IndexError:
			print("MySQL Error: %s" % str(e))

	print(logmessage)
	syslog.syslog(logmessage)
			
# -- end check_db() --

def readupsvar():
	print('\nreadupsvar()')
	
	global ups_status
	global battery_charge
	global battery_runtime
	global output_voltage
	
	try:
		# connect to UPS service
		# ups = PyNUT.PyNUTClient( host='localhost', login='admin', password='adminpass' )
		ups = PyNUTClient()
		# read all info from ups
		# UPSvars = ups.GetUPSVars( ups='eaton800' )
		UPSvars = ups.list_vars("eaton800")
		# convert it in JSON format (easier to parse with JSON module)
		UPSVarsJSON = json.dumps(UPSvars)
		# parse JSON info
		Parsed_UPSVarsJSON = json.loads(UPSVarsJSON)
		
		# store infos in global variables
		battery_charge = int(Parsed_UPSVarsJSON['battery.charge'])
		battery_runtime = int(Parsed_UPSVarsJSON['battery.runtime'])
		ups_status  = Parsed_UPSVarsJSON['ups.status']
		output_voltage = float(Parsed_UPSVarsJSON['output.voltage'])
		
		# print debug and log message
		logmessage = " ups status is " + ups_status + " \n battery charge is " + str(battery_charge) + " % \n output voltage is " + str(output_voltage) + "V \n remaining is " + str(battery_runtime) + " minutes"
		print(logmessage)
		syslog.syslog(logmessage)
	
	except:
		logmessage = " Error while reading or parsing UPS Variables"
		print(logmessage)
		syslog.syslog(logmessage)
					
# -- end readupsvar() --

def statemachine():
	print('\nstatemachine()')
	
	global SM_state
	global timeout
	global ups_status
	global battery_charge
	global battery_runtime	
	global output_voltage
	
	try:
		#initialize pushbullet 
		pb = Pushbullet('o.lMLyP43FEWjOhJqVDN5NimjD0TyjC0UH') 
		
		# Select state of state machine
		# state POWERON
		if SM_state == "POWERON" :
			print("POWERON \n")
			# if 230V is not present
			if ups_status == "OB DISCHRG":
				# change state to power outtage state
				SM_state = "POWEROUTTAGE"
				# set timeout for power outtage mode
				timeout = int(time.time()) + PERIOD_MSG * 60
				# prepare a message to send
				logmessage = time.strftime("%Y-%m-%d %H:%M:%S") + "\n panne de courant !\n Batterie a "+ str(battery_charge) +"%, "+ str(battery_runtime) +" minutes restantes."
				# print message and log it
				print(logmessage)
				syslog.syslog(logmessage)
				# send message through pushbullet to user
				push = pb.push_note("Domini - onduleur", logmessage)
				
		
		# state POWEROUTTAGE
		elif SM_state == "POWEROUTTAGE" :
			print("POWEROUTTAGE \n")
			# if 230V is present
			if ups_status == "OL CHRG":
				# change state to power on state
				SM_state = "POWERON"
				# prepare a message to send
				logmessage = time.strftime("%Y-%m-%d %H:%M:%S") + "\n Electricite revenue !\n Batterie a "+ str(battery_charge) +"%, "
				# print message and log it
				print(logmessage)
				syslog.syslog(logmessage)
				# send message through pushbullet to user
				push = pb.push_note("Domini - onduleur", logmessage)
			elif ups_status == "OB DISCHRG":
				# power outtage is still present ... wait until power recovers and send every xx minutes a status message to user
				# check if it is time to send an other status message
				now = int(time.time())
				if now > timeout :
					# prepare a message to send
					logmessage = time.strftime("%Y-%m-%d %H:%M:%S") + "\n panne de courant !\n Batterie a "+ str(battery_charge) +"%, "+ str(battery_runtime) +" minutes restantes."
					# send message through pushbullet to user
					push = pb.push_note("Domini - onduleur", logmessage)
					# print message and log it
					print(logmessage)
					syslog.syslog(logmessage)
					# set next time
					timeout = now + PERIOD_MSG * 60
	
	except :
		logmessage = " Error in state machine (probably error with pushbullet while no internet)"
		print(logmessage)
		syslog.syslog(logmessage)				
# -- end statemachine() --

def checkupsstatus():
	print('\ncheckupsstatus()')
	
	global ups_status
	global battery_charge
	global battery_runtime	
	global output_voltage
	try :
		#initialize pushbullet 
		pb = Pushbullet('o.OVDjj6Pg0u8OZMKjBVH6QBqToFbhy1ug') 
		
		# if 230V is present
		if ups_status == "OL CHRG":
			logmessage = " OK, ups is powered"
			
		# if power outtage is detected
		elif ups_status == "OB DISCHRG" :	
			logmessage = time.strftime("%Y-%m-%d %H:%M:%S") + "\n panne de courant !\n Batterie a "+ str(battery_charge) +"%, "+ str(battery_runtime) +" minutes restantes."
			# send message through pushbullet to user
			push = pb.push_note("Domini - onduleur", logmessage)
		else :
			# status unknow
			logmessage = time.strftime("%Y-%m-%d %H:%M:%S") + "\n etat inconnu : " + str(ups_status)+ ".\n Batterie a "+ str(battery_charge) +"%, "+ str(battery_runtime) +" minutes restantes."
			# send message through pushbullet to user
			push = pb.push_note("Domini - onduleur", logmessage)		
		# print message and log it
		print(logmessage)
		syslog.syslog(logmessage)	
		
	except :
		logmessage = " Error while reading or parsing UPS Variables"
		print(logmessage)
		syslog.syslog(logmessage)		
		
#todo : gerer la reprise pour eviter d'envoyer un message d'alerte trop souvent ?
	
					
# -- end checkupsstatus() --

#--- obligatoire pour lancement du code --
if __name__=="__main__": # set executable code
	setup() 	# setup() function call
	get_time()
	while True:
		readupsvar()
		statemachine()
		# loginfo()
		time.sleep(10)
	
	#checkupsstatus()
	
