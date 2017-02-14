#!/bin/sh

echo "pinging 192.168.0.1 ..."
ping -c4 192.168.0.1 > /dev/null

if [ $? != 0 ] 
then
  echo "No network connection, stopping wlan0"
  /sbin/ifdown 'wlan0'
  sleep 5
  echo "starting waln0"	
  /sbin/ifup --force 'wlan0'
else
  echo "waln0 is connected, nothing to do :-)"	
fi
