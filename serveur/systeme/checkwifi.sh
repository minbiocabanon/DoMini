#!/bin/sh

echo "pinging 192.168.0.1 ..."
ping -c4 192.168.0.1 > /dev/null

if [ $? != 0 ] 
then
	echo "No network connection, stopping wlan0"
	#sudo ip link set wlan0 down
	sudo nmcli device disconnect wlan0
	sleep 5
	echo "starting wlan0"	
	#sudo ip link set wlan0 up
	sudo nmcli device connect wlan0
	echo "udhcp request"	
	dhclient wlan0
else
  echo "wlan0 is connected, nothing to do :-)"	
fi
