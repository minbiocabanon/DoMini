#!/usr/bin/env python
import sys
import syslog

import time
import MySQLdb as mdb
from time import sleep

tables = ["chauffage_log", "voletroulant_log", "bypass_pc_log", "pellets_rsv", "ups", "internet_connex"]
NB_JOUR = 15

#--- setup ---
def setup():
	print ('Setup')
	syslog.openlog("pydel_old_data")
	syslog.syslog("Demarrage")
	logmessage = " Demarrage pydel_old_data.py, Programme qui supprime les donnees d'une date selon un nombre de jours."
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

# -- clean tables --
def clean_table():
	for table in tables:
		print "\ntable : %s" % table
		logmessage = "On supprime toutes les lignes de la table "+str(table)+" plus vieille de "+str(NB_JOUR) +" jour(s))"
		print logmessage
		syslog.syslog(logmessage)
		# Preparation de la requete MySQL
		# on ne supprime que les donnees de J-n
		try:
			# Open MySQL session
			con = mdb.connect('localhost','root','mysql','domotique')
			cur = con.cursor()
			# prepare query
			query = 'DELETE FROM `domotique`.`{0}` WHERE `date_time` < DATE_SUB(CURDATE(), interval {1} DAY);'.format(table, NB_JOUR)
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

		logmessage = 'Optimisation de la table {0} en cours ...'.format(table)
		print logmessage
		syslog.syslog(logmessage)
				
	logmessage = "Fin pydel_old_data"
	print logmessage
	syslog.syslog(logmessage)

# -- end clean_table --

# -- optimize --
def optimize():
	logmessage = 'Optimisation de la table {0} en cours ...'.format(table)
	print logmessage
	syslog.syslog(logmessage)
	for tabname in tables :
		print ' Table :',tabname
		try:
			# Open MySQL session
			con = mdb.connect('localhost','root','mysql','domotique')
			cur = con.cursor()
			# prepare query
			query = 'OPTIMIZE TABLE `{0}`;'.format(tabname)
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

#--- obligatoire pour lancement du code --
if __name__=="__main__": # set executable code
	setup()
	get_time()
	clean_table()
	optimize()