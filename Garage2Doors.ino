/*
 *  This sketch is for a garage door opener using an ESP8266-01 to control two
 *  garage doors.  This program also allows you to check to see if the doors
 *  are open or closed.  Typically the ESP8266-01 has two GPIO's available, ports 0 and 2.
 *  This sketch also uses ports 1 and 3, which are TX and RX, respectively.
 *  By using the RX and TX ports, you will not be able to get serial out once your
 *  ESP is flashed.  You can still flash it over and over if GPIO is set to LOW, but once
 *  flash is completed, disconnect your serial programmer.
 *  
 *  Created by Mark Nguyen.  Last modified: 18Dec2015
 */

#include <ESP8266WiFi.h>

const char* ssid = "my_SSID";
const char* password = "my_wifi_password";

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(10);

  // prepare GPIO2
  pinMode(0, OUTPUT);
  pinMode(1, INPUT);
  pinMode(2, OUTPUT);
  pinMode(3, INPUT);
  digitalWrite(0, 0);
  digitalWrite(2, 0);
  
  
// Connect to WiFi network
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  
// Start the server
  server.begin();

}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data
  while(!client.available()){
    delay(1);
  }
  
// Read the first line of the request
  String req = client.readStringUntil('\r');
  client.flush();

// activates the door signal if called from Web page
  if (req.indexOf("godoor1") != -1) {    
    SendPulse(0);
    WaitMessage(client);
    return;
  }  
  else if (req.indexOf("godoor2") != -1) {
    SendPulse(2);
    WaitMessage(client);
    return;
  }  

// RESTful API section, in case you want to use GET commands to activate ports
  if (req.indexOf("/door1") != -1) {
    SendPulse(0);
    client.println("<p>Sending signal to door 1</p>");
    return;
  }  
  else if (req.indexOf("/statusdoor1") != -1) {
    GetSwitchState(client,1);
    return;
  }  
       else if (req.indexOf("/door2") != -1) {
         SendPulse(2);
         client.println("<p>Sending signal to door 2</p>");
         return;
       }  
            else if (req.indexOf("/statusdoor2") != -1) {
              GetSwitchState(client,3);
              return;
            }  
  else {
  }


  client.print("<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<title>Mark's Garage Door</title>\r\n");
  client.print("<h1>Mark's Garage Door</h1>");
  client.print("<meta name=viewport content='width=400'>");

  client.println("Door 1:<br>");
  GetSwitchState(client,1);
  client.print("<form method=\"get\" action=\"/godoor1\"><button type=\"submit\" style='height:50px; width:200px'>Press Door Button</button></form><br><br>\n");
  client.print("<hr>");

  client.println("Door 2:<br>");
  GetSwitchState(client,3);
  client.print("<form method=\"get\" action=\"/godoor2\"><button type=\"submit\" style='height:50px; width:200px'>Press Door Button</button></form><br><br>\n");
  
  client.print("</html>\n");
  

  delay(1);

}

// This page has a countdown timer to allow time for door to open or close
  void WaitMessage(WiFiClient cl) {
    cl.print("<!DOCTYPE html>\r\n<html>");
    cl.print("<meta name=viewport content='width=400'>");
    cl.print("<script type=\"text/javascript\">\r\n");
    cl.print("window.onload = function() { setInterval(countdown, 1000); }\r\n");
    cl.print("function countdown() {\r\n");
    cl.print("  var num = parseInt(document.getElementById('timer').innerHTML);\r\n");
    cl.print("  if(--num < 0) location.href=document.referrer;\r\n");
    cl.print("  else document.getElementById('timer').innerHTML = num;\r\n");
    cl.print("}\r\n");
    cl.print("</script>\r\n");
    cl.print("<body>Activating the door - please wait...<br><br>\r\n");
    cl.print("This page will automatically refresh in <span id=\"timer\">14</span> seconds...");
    cl.print("</body></html>\n");
  }

// send a HIGH signal to the relay for 600ms
  void SendPulse(int doornum) {
    digitalWrite(doornum, HIGH);
    delay(600);
    digitalWrite(doornum, LOW);
  }

// check on status of a GPIO 
  void GetSwitchState(WiFiClient cl, int doornum) {
    if (digitalRead(doornum)) {
      cl.println("<p>This door is currently: OPEN</p>");
    }
    else {
      cl.println("<p>This door is currently: CLOSED</p>");
    }
  }


