#!/usr/bin/env python
import sys
import syslog
import datetime as dt
import time
import MySQLdb as mdb
from time import sleep

date_arg = dt.datetime(2017, 01, 01, 00, 00)
argexists = 0
table = "teleinfo"
NB_JOUR = 3

#--- setup ---
def setup():
	print ('Setup')
	syslog.openlog("pytribdd_edf")
	syslog.syslog("Demarrage")
	logmessage = " Demarrage pytribdd_edf.py, Programme qui allege la table teleingo en supprimant les donnees anciennes."
	print logmessage
	syslog.syslog(logmessage)
	
	global date_arg
	# if one argument is passed
	if len(sys.argv) > 1:
		# get date and format it to datetime obj
		date_arg = dt.datetime.strptime(sys.argv[1], "%Y-%m-%d").date()
	else:
		# no args , so set default date to Day-NB_JOUR
		print " Pas de date en argument"
		date_arg = dt.datetime.now() - dt.timedelta(days=NB_JOUR)

	print " datetime date_arg:",date_arg	
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

# -- tag_donnees --
def tag_donnees(table, ordre):
	global NB_JOUR
	global date_arg
	logmessage = "On lance les tags, ordre " + ordre
	print logmessage
	syslog.syslog(logmessage)

	# set variable date_tag with date to start tag operation
	date_tag = date_arg
	
	# while last date is not reached  (max date is now - NB_JOUR , in order to keep all data wihtin NB_JOUR)
	while ( date_tag.strftime("%Y-%m-%d") <= (dt.datetime.now() - dt.timedelta(days=NB_JOUR)).strftime("%Y-%m-%d")):
		print ' Jour :',date_tag.strftime("%Y-%m-%d")
		# set MySQL resquest
		# only get info for the day = date_tag
		try:
			# Open MySQL session
			con = mdb.connect('localhost','root','mysql','domotique')
			cur = con.cursor()
			# prepare query
			query = 'SELECT id FROM {0} WHERE date_format( date_time, \'%Y-%d-%m\' ) = date_format( \'{1}\', \'%Y-%d-%m\' ) ORDER BY `date_time` {2} LIMIT 0 , 1'.format(table, date_tag.strftime("%Y-%m-%d"),ordre)
			# run MySQL Query
			cur.execute(query)
			result = cur.fetchone()
			# Close all cursors
			cur.close()
			# Close MySQL session
			con.close()
			# increment date_tag + 1 day
			date_tag = date_tag + dt.timedelta(days=1)
		except mdb.Error, e:
			# create variable
			result = 0
			# Display MySQL errors
			try:
				print " MySQL Error [%d]: %s" % (e.args[0], e.args[1])
			except IndexError:
				print " MySQL Error: %s" % str(e)	
		
		try:
			#for the line return, get ID of the table
			ID = int(result[0])
			logmessage = "  ID obtenu : " + str(ID)
			print logmessage
			syslog.syslog(logmessage)
			# Open MySQL session
			con = mdb.connect('localhost','root','mysql','domotique')
			cur = con.cursor()
			# prepare query
			query = 'UPDATE `domotique`.`{0}` SET `tag` = \'1\' WHERE `{1}`.`id` ={2};'.format(table, table,ID)
			# run MySQL Query
			cur.execute(query)
			result = cur.fetchone()
			# Close all cursors
			cur.close()
			# Close MySQL session
			con.close()
			
			logmessage = "  Ligne "+ str(ID) +" taggee"
			print logmessage
			syslog.syslog(logmessage)
			
		except mdb.Error, e:
			# create variable
			result = 0
			# Display MySQL errors
			try:
				print " MySQL Error [%d]: %s" % (e.args[0], e.args[1])
			except IndexError:
				print " MySQL Error: %s" % str(e)		
	
# -- end tag_donnees --

# -- supp_donnees_tag --
def supp_donnees_tag(table):
	global NB_JOUR
	global date_arg
		
	logmessage = "On supprime toutes les lignes de la table "+ table +" qui ne sont pas taggees dans l'intervalle de temps NB_JOUR ou date passee en argument"
	print logmessage
	syslog.syslog(logmessage)
	
	# set variable date_tag with date to start tag operation
	date_tag = date_arg
	
	# while last date is not reached  (max date is now - NB_JOUR , in order to keep all data wihtin NB_JOUR)
	while ( date_tag.strftime("%Y-%m-%d") <= (dt.datetime.now() - dt.timedelta(days=NB_JOUR)).strftime("%Y-%m-%d")):
		print ' Jour :',date_tag.strftime("%Y-%m-%d")
		try:
			# Open MySQL session
			con = mdb.connect('localhost','root','mysql','domotique')
			cur = con.cursor()
			# prepare query
			query = 'DELETE FROM `domotique`.`{0}` WHERE date_format( date_time, \'%Y-%d-%m\' ) = date_format( \'{1}\', \'%Y-%d-%m\') AND `{2}`.`tag` =0;'.format(table, date_tag.strftime("%Y-%m-%d"), table)
			# run MySQL Query
			cur.execute(query)
			result = cur.fetchone()
			# Close all cursors
			cur.close()
			# Close MySQL session
			con.close()
			# increment date_tag + 1 day
			date_tag = date_tag + dt.timedelta(days=1)
		except mdb.Error, e:
			# create variable
			result = 0
			# Display MySQL errors
			try:
				print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
			except IndexError:
				print "MySQL Error: %s" % str(e)
			
# -- end tag_donnees --

# -- optimize --
def optimize():
	logmessage = 'Optimisation de la table {0} en cours ...'.format(table)
	print logmessage
	syslog.syslog(logmessage)

	try:
		# Open MySQL session
		con = mdb.connect('localhost','root','mysql','domotique')
		cur = con.cursor()
		# prepare query
		query = 'OPTIMIZE TABLE `{0}`;'.format(table)
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
# -- end optimize --
			
# -- clean tables --
def clean_table():

	# tag first of HP/HC of the day
	tag_donnees(table, "ASC")

	# tag last HP/HC of the day
	tag_donnees(table, "DESC")
	
	# delete all data not tagged during the selected day
	supp_donnees_tag(table)
					
	logmessage = "Fin pytribdd_edf"
	print logmessage
	syslog.syslog(logmessage)

# -- end clean_table --


#--- obligatoire pour lancement du code --
if __name__=="__main__": # set executable code
	setup()
	get_time()
	clean_table()
	optimize()