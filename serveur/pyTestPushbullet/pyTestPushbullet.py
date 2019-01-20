#!/usr/bin/env python
import sys
import syslog
import time
from pushbullet import Pushbullet

ACCESS_TOKEN = 'o.lMLyP43FEWjOhJqVDN5NimjD0TyjC0UH'

logmessage = ' Ceci est message de test envoye par Domini a Pushbullet (minbiocabanon)'
print logmessage
pb = Pushbullet(ACCESS_TOKEN)
msg_pb = time.strftime("%Y-%m-%d %H:%M:%S") + logmessage
push = pb.push_note("Domini", msg_pb)		



