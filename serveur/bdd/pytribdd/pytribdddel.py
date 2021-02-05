#!/usr/bin/env python
import sys
import syslog
import datetime as dt
import time
import mariadb as mdb

date_arg = dt.datetime(2017, 0o1, 0o1, 00, 00)
table = ["analog1", "analog2", "analog3", "analog4", "analog5", "analog6", "pyranometre","VMC"]
NB_JOUR = 5

#--- setup ---
def setup():
	print ('Setup')
	syslog.openlog("pytribdddel.py")
	syslog.syslog("Demarrage")
	logmessage = " Demarrage pytribdddel.py, Programme qui allege les tables capteurs en supprimant les donnees anciennes."
	print(logmessage)
	syslog.syslog(logmessage)
	
	global date_arg
	# if one argument is passed
	if len(sys.argv) > 1:
		# get date and format it to datetime obj
		date_arg = dt.datetime.strptime(sys.argv[1], "%Y-%m-%d").date()
	else:
		# no args , so set default date to Day-NB_JOUR
		logmessage =  " Pas de date en argument"
		print(logmessage)
		syslog.syslog(logmessage)
		date_arg = dt.datetime.now() - dt.timedelta(days=NB_JOUR)

	logmessage =  " datetime date_arg:" + str(date_arg)
	print(logmessage)
	syslog.syslog(logmessage)	
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

# -- tag_donnees --
def tag_donnees(table, ordre):
	global NB_JOUR
	global date_arg

	#convert for bdd syntax purpose
	if(ordre == "MIN"):
		ordre = "ASC";
	elif(ordre == "MAX"):
		ordre = "DESC";

	# for each table in table
	for tabname in table :
		logmessage = "On lance les tags "+ordre+" sur la table"+str(tabname)
		print(logmessage)
		syslog.syslog(logmessage)
		# set variable date_tag with date to start tag operation
		date_tag = date_arg
		print(' Table :',tabname)
		# while last date is not reached  (max date is now - NB_JOUR , in order to keep all data within NB_JOUR)
		while ( date_tag.strftime("%Y-%m-%d") <= (dt.datetime.now() - dt.timedelta(days=NB_JOUR)).strftime("%Y-%m-%d")):
			print(' Jour :',date_tag.strftime("%Y-%m-%d"))
			# set MySQL resquest
			# only get info for the day = date_tag
			try:
				# Open MySQL session
				con = mdb.connect(user="root",password="mysql",host="localhost",database="domotique")
				cur = con.cursor()
				# prepare query
				query = 'SELECT id, date_time, ana1 FROM `{0}` WHERE date_format( date_time, \'%Y-%d-%m\' ) = date_format( \'{1}\', \'%Y-%d-%m\' ) ORDER BY ana1 {2} LIMIT 0 , 1'.format(tabname, date_tag, ordre)
				# run MySQL Query
				cur.execute(query)
				result = cur.fetchone()
				# Close all cursors
				cur.close()
				# Close MySQL session
				con.close()
				# increment date_tag + 1 day
				date_tag = date_tag + dt.timedelta(days=1)
			except mdb.Error as e:
				# create variable
				result = 0
				# Display MySQL errors
				try:
					print(" MySQL Error [%d]: %s" % (e.args[0], e.args[1]))
				except IndexError:
					print(" MySQL Error: %s" % str(e))	
			
			try:
				# test if query has return something, if = 0 there is no line to tag so do nothing
				if(result != None):
					#for the line return, get ID of the tabname
					ID = int(result[0])
					#logmessage = "  ID obtenu : " + str(ID)
					#print logmessage
					# Open MySQL session
					con = mdb.connect(user="root",password="mysql",host="localhost",database="domotique")
					cur = con.cursor()
					# prepare query
					query = 'UPDATE `domotique`.`{0}` SET `tag` = \'1\' WHERE `{1}`.`id` ={2};'.format(tabname, tabname,ID)
					# run MySQL Query
					cur.execute(query)
					result = cur.fetchone()
					# Close all cursors
					cur.close()
					# Close MySQL session
					con.close()
					logmessage = "  Ligne "+ str(ID) +" taggee"
				else:
					logmessage = "  Pas de donnees a tagger"	
								
				print(logmessage)
				#syslog.syslog(logmessage)
				sys.stdout.flush()
				
			except mdb.Error as e:
				# create variable
				result = 0
				# Display MySQL errors
				try:
					print(" MySQL Error [%d]: %s" % (e.args[0], e.args[1]))
				except IndexError:
					print(" MySQL Error: %s" % str(e))		
		
# -- end tag_donnees --

# -- tag_donnees --
def tag_donnees_heure(table):
	global NB_JOUR
	global date_arg

	# for each table in table
	for tabname in table :
		# set variable date_tag with date to start tag operation
		date_tag = date_arg
		logmessage = "On lance les tags horaires sur la table "+str(tabname)
		print(logmessage)
		#syslog.syslog(logmessage)
		# while last date is not reached  (max date is now - NB_JOUR , in order to keep all data within NB_JOUR)
		while ( date_tag.strftime("%Y-%m-%d") <= (dt.datetime.now() - dt.timedelta(days=NB_JOUR)).strftime("%Y-%m-%d")):
			# for each hour of the day
			for h in range(0, 23):
				# set MySQL resquest
				# only get info for the day = date_tag
				try:
					# Open MySQL session
					con = mdb.connect(user="root",password="mysql",host="localhost",database="domotique")
					cur = con.cursor()
					# prepare query
					query = 'SELECT id, date_time, ana1 FROM `{0}` WHERE HOUR(date_time) = {1} AND date_format( date_time, \'%Y-%d-%m\' ) = date_format( \'{2}\', \'%Y-%d-%m\' ) ORDER BY date_time ASC LIMIT 0 , 1'.format(tabname, h, date_tag)
					# run MySQL Query
					cur.execute(query)
					result = cur.fetchone()
					# Close all cursors
					cur.close()
					# Close MySQL session
					con.close()
				except mdb.Error as e:
					# create variable
					result[0] = 0
					# Display MySQL errors
					try:
						print(" MySQL Error [%d]: %s" % (e.args[0], e.args[1]))
					except IndexError:
						print(" MySQL Error: %s" % str(e))	
				
				try:
					# test if query has return something, if = 0 there is no line to tag so do nothing
					if(result != None):
						#for the line return, get ID of the tabname
						ID = int(result[0])
						# logmessage = "  ID obtenu : " + str(ID)
						#print logmessage
						# Open MySQL session
						con = mdb.connect(user="root",password="mysql",host="localhost",database="domotique")
						cur = con.cursor()
						# prepare query
						query = 'UPDATE `domotique`.`{0}` SET `tag` = \'1\' WHERE `{1}`.`id` ={2};'.format(tabname, tabname,ID)
						# run MySQL Query
						cur.execute(query)
						result = cur.fetchone()
						# Close all cursors
						cur.close()
						# Close MySQL session
						con.close()
						
						logmessage = "  Ligne "+ str(ID) +" taggee"
					else:
						logmessage = "  Pas de donnees a tagger"
					
					print(logmessage)
					#syslog.syslog(logmessage)
					sys.stdout.flush()
					
				except mdb.Error as e:
					# create variable
					result[0] = 0
					# Display MySQL errors
					try:
						print(" MySQL Error [%d]: %s" % (e.args[0], e.args[1]))
					except IndexError:
						print(" MySQL Error: %s" % str(e))
			# increment date_tag + 1 day
			date_tag = date_tag + dt.timedelta(days=1)
# -- end tag_donnees_heure --

# -- supp_donnees_tag --
def supp_donnees_tag(table):
	global NB_JOUR
	global date_arg
		
	logmessage = "On supprime toutes les lignes de la table qui ne sont pas taggees dans l'intervalle de temps NB_JOUR ou date passee en argument"
	print(logmessage)
	syslog.syslog(logmessage)
	
	# set variable date_tag with date to start tag operation
	date_tag = date_arg
	# for each table in table
	for tabname in table :
		print(' Table :',tabname)
		try:
			# Open MySQL session
			con = mdb.connect(user="root",password="mysql",host="localhost",database="domotique")
			cur = con.cursor()
			# prepare query
			# select * from table between `lowerdate` and `upperdate`
			query = 'DELETE FROM `domotique`.`{0}` WHERE ( UNIX_TIMESTAMP(`date_time`) BETWEEN UNIX_TIMESTAMP( \'{1}\') AND UNIX_TIMESTAMP( SUBDATE(NOW(), INTERVAL {2} DAY)) ) AND `{3}`.`tag`=0'.format(tabname, date_tag.strftime("%Y-%m-%d"), NB_JOUR, tabname)
			logmessage = "  requete : " + str(query)
			print(logmessage)
			syslog.syslog(logmessage)
			# run MySQL Query
			cur.execute(query)
			con.commit()
			deletedrow = cur.rowcount
			logmessage = "   suppression de "+str(deletedrow)+" ligne(s) non taggee(s) ok."
			print(logmessage)
			syslog.syslog(logmessage)
			
		except mdb.Error as e:
			# Display MySQL errors
			try:
				print("MySQL Error [%d]: %s" % (e.args[0], e.args[1]))
			except IndexError:
				print("MySQL Error: %s" % str(e))
		
		finally:
			# closing database connection.
			# Close all cursors
			cur.close()
			# Close MySQL session
			con.close()				
# -- end tag_donnees --

# -- optimize --
def optimize():
	logmessage = 'Optimisation de la table {0} en cours ...'.format(table)
	print(logmessage)
	syslog.syslog(logmessage)
	# for each table in table
	for tabname in table :
		print(' Table :',tabname)
		try:
			# Open MySQL session
			con = mdb.connect(user="root",password="mysql",host="localhost",database="domotique")
			cur = con.cursor()
			# prepare query
			query = 'OPTIMIZE TABLE `{0}`;'.format(tabname)
			# run MySQL Query
			cur.execute(query)
			#result = cur.fetchone()
			# Close all cursors
			cur.close()
			# Close MySQL session
			con.close()
		except mdb.Error as e:
			# create variable
			#result = 0
			# Display MySQL errors
			try:
				print("MySQL Error [%d]: %s" % (e.args[0], e.args[1]))
			except IndexError:
				print("MySQL Error: %s" % str(e))
# -- end optimize --
			
# -- clean tables --
def clean_table():

	# tag MIN value of the day
	#tag_donnees(table, "MIN")

	# tag MAX value of the day
	#tag_donnees(table, "MAX")
	
	# tag last HP/HC of the day
	#tag_donnees_heure(table)
	
	# delete all data not tagged during the selected day
	supp_donnees_tag(table)

# -- end clean_table --


#--- obligatoire pour lancement du code --
if __name__=="__main__": # set executable code
	setup()
	get_time()
	clean_table()
	optimize()
	
	logmessage = "Fin pytribdddel"
	print(logmessage)
	syslog.syslog(logmessage)