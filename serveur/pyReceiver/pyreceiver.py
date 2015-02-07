import sys
import syslog
import serial
import time
import MySQLdb as mdb
from time import sleep
from threading import Thread


port = '/dev/ttyUSB0'
ser = serial.Serial(port, 57600, timeout=0)
keepThis = ''

TIMEOUTMSG  = 120 # in seconds

#--- setup --- 
def setup():
	print ('Setup')
	syslog.openlog("pyReceiver")
	syslog.syslog("Demarrage")
	print ('End Setup')
# -- fin setup -- 

# -- manage serial reception -- 
def task_receiver():
	global keepThis
	try :
		data=keepThis + ser.read(ser.inWaiting()) # read no.of bytes in waiting
		m = data.split("\n")	# get the individual lines from input
		if(len(m[-1])==0):		# true if only complete lines present (nothing after last newline)
		  processThis = m
		  keepThis = ''
		else:
		  processThis = m[0:-1]		# skip incomplete line
		  keepThis = m[-1]			# fragment
	except :
		logmessage = " ERROR while opening" + port
		print logmessage
		syslog.syslog(logmessage)

	# process the complete lines:
	for line in processThis:
		if line.startswith( '$EDF' ) :
			# exemple : $EDF,017684010,015047002,HP..,009,040,01940\r\n
			try :
				# display date/time
				print time.strftime("\n~~~~~~~~~~~~~~~~~~~~~~~~~~\n%Y-%m-%d %H:%M:%S")
				# Parse data in the message
				header, HC, HP, tarif, current, maxcurrent, VA = line.strip().split(',')
				print header, HC, HP, tarif, current, maxcurrent, VA
				# Send data in MySQL
				try:
					# Open MySQL session
					con = mdb.connect('localhost','root','mysql','domotique')
					cur = con.cursor()
					# prepare query
					query = "INSERT INTO domotique.teleinfo VALUES(NULL, NOW(), '"+ header +"', "+ HC + ", "+ HP +", '"+ tarif +"', "+ current +", "+ maxcurrent +", "+ VA +",0)"
					print " " + query
					# run MySQL Query
					cur.execute(query)
					# Close all cursors
					cur.close()
					# Close MySQL session
					con.close()
					# Print some debug
					print "  Traitement msg teleinfo  -> enreg. dans base de donnees"
				except mdb.Error, e:
					#Display MySQL errors
					try:
						print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
					except IndexError:
						print "MySQL Error: %s" % str(e)
			except :
				# if error, display header
				logmessage = " ERROR while parsing message $EDF" + line
				# log this error
				print logmessage
				syslog.syslog(logmessage)
			
		elif line.startswith( '$VMC' ) :
			# exemple : $VMC,0645,-27315,-3966,0010\r\n
			try :
				# display date/time
				print time.strftime("\n~~~~~~~~~~~~~~~~~~~~~~~~~~\n%Y-%m-%d %H:%M:%S")
				# Parse data in the message
				header, an1, an2, an3, an4 = line.strip().split(',')
				# log this data
				table = "VMC"
				print header, an1, an2, an3, an4
				# Send data in MySQL
				try:
					# Open MySQL session
					con = mdb.connect('localhost','root','mysql','domotique')
					cur = con.cursor()
					# prepare query 
					query = 'INSERT INTO domotique.{0} VALUES(NULL, NOW(), \'{1}\', {2:.2f}, {3:.2f}, {4:.2f}, {5:.2f}, 0)'.format(table, header, float(an1)/100, float(an2)/100, float(an3)/100, float(an4)/100)
					print " " + query
					# run MySQL Query
					cur.execute(query)
					# Close all cursors
					cur.close()					
					# Close MySQL session
					con.close()
					# Print some debug
					print "  Traitement msg "+ table +"  -> enreg. dans base de donnees"
				except mdb.Error, e:
					# Display MySQL errors
					try:
						print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
					except IndexError:
						print "MySQL Error: %s" % str(e)				
			except :
				logmessage = " ERROR while parsing message $VMC" + line
				print logmessage
				syslog.syslog(logmessage)
	
		elif line.startswith( '$AN1' ) or line.startswith( '$AN2' ) or line.startswith( '$AN3' ) or line.startswith( '$AN4' ) or line.startswith( '$AN5' ) or line.startswith( '$AN6' ):
			# exemple : $AN4,1509,5942,0724,0000\r\n
			try :
				# display date/time
				print time.strftime("\n~~~~~~~~~~~~~~~~~~~~~~~~~~\n%Y-%m-%d %H:%M:%S")
				# Parse data in the message
				header, an1, an2, an3, an4 = line.strip().split(',')
				# log this data
				table = "analog" + header[3]
				print header, an1, an2, an3, an4
				# Send data in MySQL
				try:
					# Open MySQL session
					con = mdb.connect('localhost','root','mysql','domotique')
					cur = con.cursor()
					# prepare query 
					query = 'INSERT INTO domotique.{0} VALUES(NULL, NOW(), \'{1}\', {2:.2f}, {3:.2f}, {4:.2f}, {5:.2f}, 0)'.format(table, header, float(an1)/100, float(an2)/100, float(an3)/100, float(an4)/100)
					print " " + query
					# run MySQL Query
					cur.execute(query)
					# Close all cursors
					cur.close()					
					# Close MySQL session
					con.close()
					# Print some debug
					print "  Traitement msg "+ table +"  -> enreg. dans base de donnees"
				except mdb.Error, e:
					# Display MySQL errors
					try:
						print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
					except IndexError:
						print "MySQL Error: %s" % str(e)				
			except :
				logmessage = " ERROR while parsing message $AN" + line
				print logmessage
				syslog.syslog(logmessage)
				
		elif line.startswith( '$PYR' ) :
			# exemple : $PYR,01515\r\n
			try :
				# display date/time
				print time.strftime("\n~~~~~~~~~~~~~~~~~~~~~~~~~~\n%Y-%m-%d %H:%M:%S")
				# Parse data in the message
				header, an1 = line.strip().split(',')
				# log this data
				table = "pyranometre"
				print header, an1
				# Send data in MySQL
				try:
					# Open MySQL session
					con = mdb.connect('localhost','root','mysql','domotique')
					cur = con.cursor()
					# prepare query 
					query = 'INSERT INTO domotique.{0} VALUES(NULL, NOW(), \'{1}\', {2:.2f}, 0)'.format( table, header, float(an1)/100 )
					print " " + query
					# run MySQL Query
					cur.execute(query)
					# Close all cursors
					cur.close()					
					# Close MySQL session
					con.close()
					# Print some debug
					print "  Traitement msg "+ table +"  -> enreg. dans base de donnees"
				except mdb.Error, e:
					# Display MySQL errors
					try:
						print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
					except IndexError:
						print "MySQL Error: %s" % str(e)				
			except :
				logmessage = " ERROR while parsing message $PYR : " + line
				print logmessage
				syslog.syslog(logmessage)
				
		elif line.startswith( '$POL' ) or line.startswith( '!POL' ):
			# exemple : $POL,MOD,0\r\n
			# exemple : $POL,SAC,1\r\n
			# exemple : $POL,NVG,54\r\n
			try :
				# display date/time
				print time.strftime("\n~~~~~~~~~~~~~~~~~~~~~~~~~~\n%Y-%m-%d %H:%M:%S")
				# Parse data in the message
				header, typmsg, var1 = line.strip().split(',')
				# log this data
				logmessage = " " + header + " received, type message : " + typmsg + ", var1 : " + var1
				syslog.syslog(logmessage)
				print header, typmsg, var1
				# if it is an message about pellet bag
				if ( typmsg == "SAC" ) :
					# Send data in MySQL
					try:
						# Open MySQL session
						con = mdb.connect('localhost','root','mysql','domotique')
						cur = con.cursor()
						# prepare query to recover number of pellet bag in stock
						query = 'SELECT capital FROM pellets ORDER BY id DESC LIMIT 0 , 1'
						print " " + query
						# run MySQL Query
						cur.execute(query)
						result = cur.fetchone()
						stock_pellet = result[0]
						logmessage = ' Nb de sacs en stock: {}'.format(stock_pellet)
						print logmessage
						syslog.syslog(logmessage)
						
						# Prepare a new query to save new value of stock
						query = 'INSERT INTO domotique.pellets VALUES(NULL, NOW(), 0, {}, {})'.format( int(var1), (int(stock_pellet) - int(var1)))
						print " " + query
						# run MySQL Query
						cur.execute(query)						
						# Close all cursors
						cur.close()						
						# Close MySQL session
						con.close()
						# Print some debug
						print "  Traitement msg $POL,SAC  -> enreg. dans base de donnees"
					except mdb.Error, e:
						# Display MySQL errors
						try:
							print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
						except IndexError:
							print "MySQL Error: %s" % str(e)
				elif ( typmsg == "MOD" ) :
					# Send data in MySQL
					try:
						# Open MySQL session
						# con = mdb.connect('localhost','root','mysql','domotique')
						# cur = con.cursor()
						# # prepare query to recover number of pellet bag in stock
						# query = 'SELECT capital FROM pellets ORDER BY id DESC LIMIT 0 , 1'
						# print " " + query
						# # run MySQL Query
						# cur.execute(query)
						# result = cur.fetchone()
						# stock_pellet = result[0]
						# logmessage = ' Nb de sacs en stock: {}'.format(stock_pellet)
						# print logmessage
						# syslog.syslog(logmessage)
						
						# # Prepare a new query to save new value of stock
						# query = 'INSERT INTO domotique.pellets VALUES(NULL, NOW(), 0, {}, {})'.format( int(var1), (int(stock_pellet) - int(var1)))
						# print " " + query
						# # run MySQL Query
						# # cur.execute(query)						
						# # Close all cursors
						# cur.close()						
						# # Close MySQL session
						# con.close()
						# Print some debug
						print "  Traitement msg $POL,MOD  -> pas d'action pour le moment"
					except mdb.Error, e:
						# Display MySQL errors
						try:
							print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
						except IndexError:
							print "MySQL Error: %s" % str(e)
				elif ( typmsg == "NVG" ) :
					# Send data in MySQL
					try:
						# Open MySQL session
						con = mdb.connect('localhost','root','mysql','domotique')
						cur = con.cursor()
						logmessage = ' Niveau de granules dans le reservoir: {0:3d}'.format(int(var1))
						print logmessage
						syslog.syslog(logmessage)
						# Prepare a new query to save new value of stock
						query = 'INSERT INTO domotique.pellets_rsv VALUES(NULL, NOW(), \'{0}\', {1:3d}, 0)'.format( header, int(var1))
						print " " + query
						# run MySQL Query
						cur.execute(query)						
						# Close all cursors
						cur.close()						
						# Close MySQL session
						con.close()
						# Print some debug
						print "  Traitement msg $POL,NVG  -> enreg. dans base de donnees"
					except mdb.Error, e:
						# Display MySQL errors
						try:
							print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
						except IndexError:
							print "MySQL Error: %s" % str(e)
				else :
					print "ici"
				
			except :
				# if error, display header
				logmessage = " ERROR while parsing message $POL" + line
				print logmessage
				# log this error
				syslog.syslog(logmessage)	

		else :
			# display received message (not recognized but usefull for debug)
			if len( line ) > 0 :
				# display date/time
				print time.strftime("\n~~~~~~~~~~~~~~~~~~~~~~~~~~\n%Y-%m-%d %H:%M:%S")
				try :
					logmessage = " WARNING : Unknown message received : " + line
					print logmessage
					syslog.syslog(logmessage)
				except :
					logmessage = " ERROR while parsing unknown message "
					print logmessage
					syslog.syslog(logmessage)
# -- end task_receiver --

# -- manage serial emission -- 
def task_emitter():
	# Check messages in the MySQL stack
	try:
		# Open MySQL session
		con = mdb.connect('localhost','root','mysql','domotique')
		cur = con.cursor()
		# prepare query 
		query = 'SELECT id, date_time, UNIX_TIMESTAMP(date_time) AS date_unix, message FROM `domotique`.`tx_msg_radio` ORDER BY date_time ASC'
		# run MySQL Query
		cur.execute(query)
		messages = cur.fetchall()		
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

	# for each messages in the stack
	try :
		for message in messages:
			#check if timestamp is not to old 
			# display date/time
			print time.strftime("\n~~~~~~~~~~~~~~~~~~~~~~~~~~\n%Y-%m-%d %H:%M:%S")		
			if (time.time() - message[2] < TIMEOUTMSG) :
				#send this message on the serial port
				try:
					# write message to serial port
					msg = message[3] + "\n" # add LF at the end of the message, because Jeelink need it as a trigger caracter
					ser.write(msg)
					logmessage = " Emitter_task : message send to serial : " + str(message[3])
					print logmessage
					syslog.syslog(logmessage)
				except:
					# if error, print error returned
					logmessage = " Emitter_task : ERROR in sending this message over serial : " + str(message[3])
					print logmessage
					syslog.syslog(logmessage)			
			else :
				# Message has expired
				logmessage = " Emitter_task : message has expired : " + str(message[3]) + " date/time :" + str(message[1]) + "-" + str(message[2])
				print logmessage
				syslog.syslog(logmessage)			
			
			# delete this message
			try:
				# Open MySQL session
				con = mdb.connect('localhost','root','mysql','domotique')
				cur = con.cursor()
				# prepare query 
				query = 'DELETE FROM `domotique`.`tx_msg_radio` WHERE `tx_msg_radio`.`id` = {0};'.format(message[0])
				# run MySQL Query
				cur.execute(query)		
				# Close all cursors
				cur.close()					
				# Close MySQL session
				con.close()	
				# Add some log messages
				logmessage = " Emitter_task : message has been deleted : " + str(message[3]) + " id :" + str(message[0])
				print logmessage
				syslog.syslog(logmessage)			
			except mdb.Error, e:
				# Display MySQL errors
				try:
					print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
				except IndexError:
					print "MySQL Error: %s" % str(e)		
	except :
		# Add some log messages
		logmessage = " Emitter_task : error with MySQL or messages variable content"
		print logmessage
		syslog.syslog(logmessage)		
# -- end task_emitter --


#--- obligatoire pour lancement du code -- 
if __name__=="__main__": # set executable code
	setup() 	# setup() function call
	while(1): 	# infinite loop
		task_receiver()
		task_emitter()
		
		sleep(1)
		print ('.'),
		sys.stdout.flush()
	ser.close()