
//DIN Grey  D7
//CS  White D4 
//CLK Black D5

// MAX7219 FC16


#include <ESP8266WiFi.h>            // ESP8266 Wifi library
#include <ESP8266WebServer.h>       // Web server functions
#include <ESP8266mDNS.h>            // Web server functions
#include <SPI.h>
#include <bitBangedSPI.h>
#include <MAX7219_Dot_Matrix.h>

const byte chips = 4;               // Number of 8x8 modules linked together
const int msglen = 500;             // Maximum length of the message

// Set your wifi credetials here, for open wifi leave password empty
const char* ssid = "SECRET NETWORK";
const char* password = "xxxxxx";

// You can comment this section out for using DHCP
//IPAddress ip(192, 168, 1, 85); // where xx is the desired IP Address
//IPAddress gateway(192, 168, 0, 1); // set gateway to match your network
//IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your network

// n chips (display modules), hardware SPI with load on GPIO2
MAX7219_Dot_Matrix display (chips, 2);  // Chips / LOAD 
WiFiClient espClient;
MDNSResponder mdns;
ESP8266WebServer server(80);

char message [msglen] = "Visit website to customize";
unsigned long lastMoved = 0;
unsigned long MOVE_INTERVAL = 40;  // Default delay in miliseconds
byte intensity = 4;                // Default intensity 0-15

int  messageOffset;
String webPage = "";
String webStat = "";
String temp = "";
 
void setup() {
  Serial.begin(115200);
  display.begin();
  display.setIntensity(intensity);
  display.sendSmooth ("", 0);

  // Adjust to your own needs

  

//  WiFi.config(ip, gateway, subnet);   // Comment this line for using DHCP
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  temp = "silahkan Hubungkan ke http://";
  temp += WiFi.localIP().toString();
  temp +=" ";
  temp.toCharArray(message, msglen);
  Serial.print("Signal [RSSI]: ");
  Serial.println(WiFi.RSSI());

  if (mdns.begin("max7219_display", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }  
  server.on("/", handleMainPage);
  server.on("/set", handleSetCommand);        // Handling parameter set
  server.begin();
  Serial.println("HTTP server started");
  messageOffset = - chips * 8;
}  // end of setup



void handleMainPage() {
  
  webPage = "<!DOCTYPE html>\n"
"<html>\n"
"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
"<link href=\"https://fonts.googleapis.com/css?family=Comfortaa|Cormorant+Garamond|Poiret+One\" rel=\"stylesheet\">\n"
"<link rel=\"stylesheet\" href=\"https://www.w3schools.com/w3css/4/w3.css\">\n"
"<style>\n"
"input[type=text], select {\n"
"  width: 100%;\n"
"  padding: 12px 20px;\n"
"  margin: 8px 0;\n"
"  display: inline-block;\n"
"  border: 1px solid #ccc;\n"
"  border-radius: 4px;\n"
"  box-sizing: border-box;\n"
"}\n"
"\n"
"input[type=submit] {\n"
"  width: 100%;\n"
"  background-color: #4CAF50;\n"
"  color: white;\n"
"  padding: 14px 20px;\n"
"  margin: 8px 0;\n"
"  border: none;\n"
"  border-radius: 4px;\n"
"  cursor: pointer;\n"
"}\n"
"\n"
"input[type=submit]:hover {\n"
"  background-color: #45a049;\n"
"}\n"
"\n"
"div {\n"
"  border-radius: 5px;\n"
"  background-color: #f2f2f2;\n"
"  padding: 20px;\n"
"  font-family: 'Comfortaa', cursive;\n"
"}\n"
"\n"
"#anak {\n"
"  border-radius: 5px;\n"
"  background-color: #fbe8ec;\n"
"  padding: 20px;\n"
"  font-family: 'Comfortaa', cursive;\n"
"}\n"
"\n"
"#anakk {\n"
"  border-radius: 5px;\n"
"  background-color: #cdd3cc;\n"
"  padding: 20px;\n"
"  font-family: 'Comfortaa', cursive;\n"
"}\n"
"\n"
"h3 {\n"
"  font-family: 'Comfortaa', cursive;\n"
"}\n"
"h2 {\n"
"  font-family: 'Comfortaa', cursive;\n"
"}\n"
"b {\n"
"  font-family: 'Comfortaa', cursive;\n"
"}\n"
"p {\n"
"  font-family: 'Comfortaa', cursive;\n"
"}\n"
"\n"
"</style>\n"
"<body>\n"
"<center>\n"
"<h2>TEXT BERJALAN WIFI</h2>\n"
"\n"
"\n"
"<div>\n"
"  <form action=\"set\">\n"
"    <h2 >Isi Pesan</h2>\n"
"    <input type='text' name='text' > \n"
"     \n"
"    <input type=\"submit\" value=\"Kirim\">\n"
"  </form>\n"
"</div>\n"
"<div class=\"w3-container\" id=\"anak\">\n"
"  <h3>KECEPATAN SCROLL</h3>\n"
"  </br>\n"
"  <a href=\"set?delay=10\" class=\"w3-btn w3-green\">10</a>\n"
"  <a href=\"set?delay=20\" class=\"w3-btn w3-green\">20</a>\n"
"  <a href=\"set?delay=30\" class=\"w3-btn w3-green\">30</a>\n"
"  <a href=\"set?delay=40\" class=\"w3-btn w3-green\">40</a>\n"
"  <a href=\"set?delay=50\" class=\"w3-btn w3-green\">50</a><br>\n"
"  </br>\n"
"  <a href=\"set?delay=60\" class=\"w3-btn w3-green\">60</a>\n"
"  <a href=\"set?delay=70\" class=\"w3-btn w3-green\">70</a>\n"
"  <a href=\"set?delay=80\" class=\"w3-btn w3-green\">80</a>\n"
"  <a href=\"set?delay=90\" class=\"w3-btn w3-green\">90</a>\n"
"  <a href=\"set?delay=100\" class=\"w3-btn w3-green\">100</a>\n"
"</div>\n"
"<div class=\"w3-container\" id=\"anakk\">\n"
"  <h3>KECERAHAN</h3>\n"
"  </br>\n"
"  <a href=\"set?intensity=1\" class=\"w3-btn w3-green\">1</a>\n"
"  <a href=\"set?intensity=2\" class=\"w3-btn w3-green\">2</a>\n"
"  <a href=\"set?intensity=3\" class=\"w3-btn w3-green\">3</a>\n"
"  <a href=\"set?intensity=4\" class=\"w3-btn w3-green\">4</a>\n"
"  <a href=\"set?intensity=5\" class=\"w3-btn w3-green\">5</a><br>\n"
"  </br>\n"
"  <a href=\"set?intensity=6\" class=\"w3-btn w3-green\">6</a>\n"
"  <a href=\"set?intensity=7\" class=\"w3-btn w3-green\">7</a>\n"
"  <a href=\"set?intensity=8\" class=\"w3-btn w3-green\">8</a>\n"
"  <a href=\"set?intensity=9\" class=\"w3-btn w3-green\">9</a>\n"
"  <a href=\"set?intensity=10\" class=\"w3-btn w3-green\">10</a><br>\n"
"  </br>\n"
"  <a href=\"set?intensity=11\" class=\"w3-btn w3-green\">11</a>\n"
"  <a href=\"set?intensity=12\" class=\"w3-btn w3-green\">12</a>\n"
"  <a href=\"set?intensity=13\" class=\"w3-btn w3-green\">13</a>\n"
"  <a href=\"set?intensity=14\" class=\"w3-btn w3-green\">14</a>\n"
"  <a href=\"set?intensity=15\" class=\"w3-btn w3-green\">15</a>\n"
"</div>\n"
"\n"
"</center>\n"
"</body>\n"
"</html>\n"
"";

  webPage +="<center>\n";
  webPage += "<div>\n";
  webPage += "<p><b>Kecerahan Sekarang: ";
  webPage += intensity;
  webPage += "<p><b>Kecepatan Sekarang: ";
  webPage += MOVE_INTERVAL;
  webStat +="</div>\n";
      
  server.send(200, "text/html", webPage+webStat); 
  Serial.println("Web page request");
}

void handleSetCommand() { 
  String response = "";
  if (server.args() == 0) {
    response = "Salah bro";
  } else {
    if (server.argName(0) == "delay") {
      MOVE_INTERVAL = server.arg("delay").toInt();
      response = "Kecepatan Kecerahan di update ke ";
      response += MOVE_INTERVAL;
    }
    if (server.argName(0) == "intensity") {
      intensity = (byte)server.arg("intensity").toInt();
      response = "Kecerahan di update ke ";
      display.setIntensity(intensity);
      response += intensity;
    }
    if (server.argName(0) == "text") {
      server.arg("text").toCharArray(message, msglen);
      messageOffset = - chips * 8;
      response = "Pesan di update";
    }
    if (response == "" ) {
      response = "Salah bro";
    }
  }
  Serial.print("Change request: ");
  Serial.println(response);
  response = "<html><head><meta http-equiv=\"refresh\" content=\"2; url=/\"></head><body>" + response + "</body></html>";
  server.send(200, "text/html", response);          //Returns the HTTP response
}


void updateDisplay() {
 display.sendSmooth (message, messageOffset);
//display.sendString ("dissssss");


  // next time show one pixel onwards
  if (messageOffset++ >= (int) (strlen (message) * 8))
    messageOffset = - chips * 8;
}  // end of updateDisplay

void loop() { 


    
  // update display if time is up
  if (millis () - lastMoved >= MOVE_INTERVAL) {
    updateDisplay ();
    lastMoved = millis ();
  }

  // Handle HTTP server requests
  server.handleClient();
     
}  // end of loop
