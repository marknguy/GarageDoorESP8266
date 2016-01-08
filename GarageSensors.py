#!/usr/bin/python

# Polls the ESP8266 sensor continuously for door contact status.
# This script provides an example of pull data using HTML calls.  The
# pull is using a REST-like method, but not truly REST (because the web
# ESP8266 does not implement full REST.  The script then parses the response 
# for the appropriate date (in case the status of the sensor).  Then the 
# script pushes the data to the server using a RESTful call (in this case the
# server does support REST).  This process is repeats over and over unless
# there is an issue with the ESP8266.
# Mark Nguyen     last updated 7-jan-2016

import json, requests, time

# URLs that request door sensor status
url1 = 'http://192.168.27.79/statusdoor1'
url2 = 'http://192.168.27.79/statusdoor2'

# REST API for OpenHab server
OHurl1 = 'http://10.1.1.70:8080/rest/items/Garage1Sensor/state'
OHurl2 = 'http://10.1.1.70:8080/rest/items/Garage2Sensor/state'


# boolean to check if a valid response is received from ESP8266
perform = True

# loop
while(perform):
# reads response from query
 response1 = requests.get(url=url1)
# parses the JSON data
 data1 = json.loads(response1.text)
# retrieves the resultant data associated with parameter “status”
 status1 = data1["status"]
# checks sensor state
 if ((status1 == 'OPEN')|(status1 == 'CLOSED')):
# REST put call to server
    requests.put(OHurl1,data=status1) 
    time.sleep(10)
 else:
    perform = False
 response2 = requests.get(url=url2)
 data2 = json.loads(response2.text)
 status2 = data2["status"]
 if ((status2 == 'OPEN')|(status2 == 'CLOSED')):
    requests.put(OHurl2,data=status2) 
    time.sleep(10)
 else:
    perform = False
