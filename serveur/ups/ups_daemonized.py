#!/usr/bin/env python
import sys
import syslog
import time
from pushbullet import Pushbullet
import PyNUT
import json
import MySQLdb as mdb

import daemon

# -- USER variables
PERIOD_MSG = 10		# in minutes, delay two messages with status info while power outtage
PERIOD_LOG = 10		# in minutes, delay two messages in syslog (debug only)
# -- Global variables
ups_status = "OL CHG"
battery_charge = 100 
battery_runtime = 1024
output_voltage = 230.0
SM_state = "POWERON"
timeout = 0
timeoutlog = 0

#--- setup ---
def setup():
	print ('Setup')
	syslog.openlog("pyUps")
	syslog.syslog("Demarrage")
	timeoutlog = int(time.time())
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

def readupsvar():
	print '\nreadupsvar()'
	
	global ups_status
	global battery_charge
	global battery_runtime
	global output_voltage
	
	try:
		# connect to UPS service
		ups = PyNUT.PyNUTClient( host='localhost', login='admin', password='adminpass' )
		# read all info from ups
		UPSvars = ups.GetUPSVars( ups='eaton800' )
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
		print logmessage
	
	except:
		logmessage = " Error while reading or parsing UPS Variables"
		print logmessage

# -- end readupsvar() --

def statemachine():
	print '\nstatemachine()'
	
	global SM_state
	global timeout
	global ups_status
	global battery_charge
	global battery_runtime	
	global output_voltage
	global PERIOD_MSG
	
	try:
		#initialize pushbullet 
		pb = Pushbullet('o.OVDjj6Pg0u8OZMKjBVH6QBqToFbhy1ug') 
		
		# Select state of state machine
		# state POWERON
		if SM_state == "POWERON" :
			print "POWERON \n"
			# if 230V is not present
			if ups_status == "OB DISCHRG":
				# change state to power outtage state
				SM_state = "POWEROUTTAGE"
				# set timeout for power outtage mode
				timeout = int(time.time()) + PERIOD_MSG * 60
				# prepare a message to send
				logmessage = time.strftime("%Y-%m-%d %H:%M:%S") + "\n panne de courant !\n Batterie a "+ str(battery_charge) +"%, "+ str(battery_runtime) +" minutes restantes."
				# print message and log it
				print logmessage
				syslog.syslog(logmessage)
				# send message through pushbullet to user
				push = pb.push_note("Domini - onduleur", logmessage)
				
		
		# state POWEROUTTAGE
		elif SM_state == "POWEROUTTAGE" :
			print "POWEROUTTAGE \n"
			# if 230V is present
			if ups_status == "OL CHRG":
				# change state to power on state
				SM_state = "POWERON"
				# prepare a message to send
				logmessage = time.strftime("%Y-%m-%d %H:%M:%S") + "\n Electricite revenue !\n Batterie a "+ str(battery_charge) +"%, "
				# print message and log it
				print logmessage
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
					print logmessage
					syslog.syslog(logmessage)
					# set next time
					timeout = now + PERIOD_MSG * 60
	
	except :
		logmessage = " Error in state machine (probably error with pushbullet while no internet)"
		print logmessage
		syslog.syslog(logmessage)
# -- end statemachine() --

def checkupsstatus():
	print '\ncheckupsstatus()'
	
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
		print logmessage
		syslog.syslog(logmessage)
		
	except :
		logmessage = " Error while reading or parsing UPS Variables"
		print logmessage
		syslog.syslog(logmessage)
		
# -- end checkupsstatus() --
		
def loginfo():
	print '\nloginfo()'
	
	global ups_status
	global battery_charge
	global battery_runtime	
	global output_voltage
	global timeoutlog
	global PERIOD_LOG
	
	# Log info in syslog
	try :
		# check if it is time to send an other status message
		now = int(time.time())
		if now > timeoutlog :
			# prepare a message to send
			logmessage = time.strftime("%Y-%m-%d %H:%M:%S") + " Etat onduleur : " + ups_status + ", Batterie a "+ str(battery_charge) +"%, "+ str(battery_runtime) +" minutes restantes."
			# log message
			syslog.syslog(logmessage)
			# set next time
			timeoutlog = now + PERIOD_LOG * 60
			
			# Log info in database
			try:
				# Open MySQL session
				con = mdb.connect('localhost','root','mysql','domotique')
				cur = con.cursor()
				# prepare query
				#query = "INSERT INTO domotique.ups VALUES (NULL, NOW() , '"+ ups_status +"', '"+ str(battery_charge) +"', '"+ str(battery_runtime) +"', '"+ str(output_voltage) +"');"
				query = 'INSERT INTO domotique.ups VALUES (NULL, NOW(), \'{0}\', \'{1}\', \'{2}\', \'{3}\',0);'.format(ups_status, battery_charge, battery_runtime, output_voltage)
				# run MySQL Query
				cur.execute(query)
				# Make sure data is committed to the database
				con.commit()
				# Close all cursors
				cur.close()
				# Close MySQL session
				con.close()
				# say that request is ok
				logmessage = " Data saved in ups table."
				syslog.syslog(logmessage)
			except mdb.Error, e:
				# Display MySQL errors
				try:
					print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
				except IndexError:
					print "MySQL Error: %s" % str(e)			
			
	except :
		logmessage = " Error in loginfo()"
		syslog.syslog(logmessage)
		
# -- end loginfo() --
	


def ups_monitor():
	setup()
	get_time()
	while True:
		readupsvar()
		statemachine()
		loginfo()
		time.sleep(10)

def run():
    with daemon.DaemonContext():
        ups_monitor()

if __name__ == "__main__":
    run()