/*
 *  This sketch is for a garage door opener using an ESP8266-01 to control one
 *  relay and one sensor.  GPIO 2 is used for the relay control.  GPIO 3 (which is 
 *  also RX) is used for the sensor.
 *  GPIO 1, which is also TX, can be still be used as the serial output for troubleshooting
 *  
 *  Created by Mark Nguyen.  Last modified: 14Jan2016
 */

#include <ESP8266WiFi.h>

const char* ssid = "myssid";
const char* password = "mypassword";

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(10);

  // prepare GPIO2 and GPIO3
  pinMode(3, INPUT);
  pinMode(2, OUTPUT);
  digitalWrite(2, 0);
  
  
// Connect to WiFi network

  WiFi.mode(WIFI_STA);

// Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  
  
  
// Start the server
  server.begin();
  Serial.println("Server started");


  // Print the IP address
  Serial.println(WiFi.localIP());

}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data
  while(!client.available()){
    delay(100);
  }
  
// Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

// activates the door signal if called from Web page
  if (req.indexOf("godoor1") != -1) {    
    SendPulse(2);
    WaitMessage(client);
    client.stop();
    return;
  }  

// Prepare the respose
String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";

// REST-like API section, in case you want to use GET commands to activate ports
  if (req.indexOf("/door1") != -1) {
    SendPulse(2);
// width reduced to accommodate mobile devices    
    s += "\r\n<!DOCTYPE HTML>\r\n<html>\r\n<body>\r\n<meta name=viewport content='width=400'>\r\n";
    s += "<p>Sending signal to door 1</p>";
    s += "</body></html>\n";
    client.print(s);
    client.stop();
    return;
  }
// Returns status of sensor in JSON format      
  else if (req.indexOf("/statusdoor1") != -1) {
    if (digitalRead(3)) {
      s += "{ \"status\": \"OPEN\" }";
    }
    else {
      s += "{ \"status\": \"CLOSED\" }";
    }
    client.print(s);
    client.stop();
    return;
  }  
  else {
  }

// Main page
  String m = "<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<title>Mark's Garage Door</title>\r\n";
  m += "<h1>Mark's Garage Door</h1>";
  m += "<meta name=viewport content='width=400'>";
  m += "Door 1:<br>";
// calls a function that returns a String with the state of the switch  
  m += GetSwitchState(client,3);
  m += "<form method=\"get\" action=\"/godoor1\"><button type=\"submit\" style='height:50px; width:200px'>Press Door Button</button></form><br><br>\n";
  m += "<hr>";
  
  m += "</html>\n";
  client.print(m);
  

  delay(1);

}

// This page has a countdown timer to allow time for door to open or close
  void WaitMessage(WiFiClient cl) {
// Prepare the response
    String response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    response += "<html><meta name=viewport content='width=400'>\r\n";
    response += "<script type=\"text/javascript\">\r\n";
    response += "window.onload = function() { setInterval(countdown, 1000); }\r\n";
    response += "function countdown() {\r\n";
    response += "  var num = parseInt(document.getElementById('timer').innerHTML);\r\n";
    response += "  if(--num < 0) location.href=document.referrer;\r\n";
    response += "  else document.getElementById('timer').innerHTML = num;\r\n";
    response += "}\r\n</script>\r\n";
    response += "<body>Activating the door - please wait...<br><br>\r\n";
    response += "This page will automatically refresh in <span id=\"timer\">14</span> seconds...";
    response += "</body></html>\n";
    cl.print(response);
  }

// send a HIGH signal to the relay for 500ms, which simulates a button
  void SendPulse(int doornum) {
    digitalWrite(doornum, HIGH);
    delay(500);
    digitalWrite(doornum, LOW);
  }

// check on status of a GPIO 
  String GetSwitchState(WiFiClient cl, int doornum) {
    if (digitalRead(doornum)) {
      return("<p>This door is currently: OPEN</p>");
    }
    else {
      return("<p>This door is currently: CLOSED</p>");
    }   
  }

