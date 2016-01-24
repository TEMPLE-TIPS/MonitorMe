#!/bin/bash
{
echo From: devel.monitorme@gmail.com
echo Subject: ALERT FROM MonitorME!
echo
echo "We have a problem. Our systems have detected a distrubance with"\
    "your MonitorMe camera.\n\n"\
    "-- MonitorMe Team"

} | ssmtp $1 -F"MonitorMe" 
