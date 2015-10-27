#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
//a library for json manipulation
#include <ArduinoJson.h>

ESP8266WebServer server(80);

const char* localSsid = "test";
const char* localPassphrase = "test";
String st;
String content;
int statusCode;
int workingMode = 1; //0:is client to AP; 1:is AP

//node application
char* host = "               ";
String espId;
int clientId = 1;
int clientVer = 0;
//the pin where the led is connected
const int pin        = 13;
const int inputPin   = 12;

//server ip start point in eeprom
const int srvIpStartPos = 495;
const int espIdStartPos = 492;

void setup() {
  pinMode(pin, OUTPUT);
  pinMode(inputPin, INPUT);
  Serial.begin(115200);
  EEPROM.begin(512);
  delay(10);
  Serial.println();
  Serial.println("Startup");
  // read eeprom for ssid and pass
  Serial.println("Reading EEPROM ssid");
  String esid;
  for (int i = 0; i < 32; ++i)
    {
      esid += char(EEPROM.read(i));
    }
  Serial.print("SSID: ");
  Serial.println(esid);
  Serial.println("Reading EEPROM pass");
  String epass = "";
  for (int i = 32; i < 96; ++i)
    {
      epass += char(EEPROM.read(i));
    }
 for (int i = 0; i < 3; ++i)
    {
      espId+= char(EEPROM.read(espIdStartPos+i));
    }
 for (int i = 0; i < 15; ++i)
    {
      host[i]= char(EEPROM.read(srvIpStartPos+i));
    }
  //Serial.println("host: "+ String(host));
  espId = espId.substring(0,3);
 // Serial.println("espId: "+espId);
    
  
  if ( esid.length() > 1 ) {
      WiFi.begin(esid.c_str(), epass.c_str());
      if (testWifi()) {
        workingMode = 0;
        launchWeb(0);
        return;
      } 
  }
  setupAP();
}

bool testWifi(void) {
  int c = 0;
  Serial.println("Waiting for Wifi to connect");  
  while ( c < 20 ) {
    if (WiFi.status() == WL_CONNECTED) { return true; } 
    delay(500);
    Serial.print(WiFi.status());    
    c++;
  }
  Serial.println("");
  Serial.println("Connect timed out, opening AP");
  return false;
} 

void launchWeb(int webtype) {
  Serial.println("WiFi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  createWebServer(webtype);
  // Start the server
  server.begin();
  Serial.println("Server started at "+ String(host)); 
}

void setupAP(void) {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
     {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
      delay(10);
     }
  }
  Serial.println(""); 
  st = "<ol>";
  for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      st += "<li>";
      st += WiFi.SSID(i);
      st += " (";
      st += WiFi.RSSI(i);
      st += ")";
      st += (WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*";
      st += "</li>";
    }
  st += "</ol>";
  delay(100);
  WiFi.softAP(localSsid); //, localPassphrase, 6);
  delay(1000);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("softap");
  Serial.println(myIP);
  launchWeb(1);
  Serial.println("over");
}

void createWebServer(int webtype)
{
  if ( webtype == 1 ) {
    server.on("/", []() {
        IPAddress ip = WiFi.softAPIP();
        String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
        content = "<!DOCTYPE HTML>\r\n<html>Hello from ESP8266 at ";
        content += ipStr;
        content += "<p>";
        content += st;
        content += "</p><form method='get' action='setting'><label>SSID: </label><input name='ssid' length=32><input name='pass' length=64><input type='submit'></form>";
        content += "</html>";
        server.send(200, "text/html", content);  
    });
    server.on("/setting", []() {
        String qsid = server.arg("ssid");
        String qpass = server.arg("pass");
        String qsrvIp = server.arg("srvIp");
        String qespId = server.arg("espId");
        if (qsid.length() > 0 && qpass.length() > 0 && qsrvIp.length()>0 && qespId.length()>0) {
          Serial.println("clearing eeprom");
          for (int i = 0; i < 96; ++i) { EEPROM.write(i, 0); }
          //Serial.println(qsid);
          //Serial.println("");
          //Serial.println(qpass);
          //Serial.println("");
            
          Serial.println("writing eeprom ssid:");
          for (int i = 0; i < qsid.length(); ++i)
            {
              EEPROM.write(i, qsid[i]);
              //Serial.print("Wrote: ");
              //Serial.println(qsid[i]); 
            }
          Serial.println("writing eeprom pass:"); 
          for (int i = 0; i < qpass.length(); ++i)
            {
              EEPROM.write(32+i, qpass[i]);
              //Serial.print("Wrote: ");
              //Serial.println(qpass[i]); 
            }  
          Serial.println("writing eeprom srvIp:"); 
         //clear contents
          for (int i = srvIpStartPos; i < srvIpStartPos+15; ++i) { EEPROM.write(i, 0); }
          for (int i = 0; i < qsrvIp.length(); ++i)
            {
              EEPROM.write(srvIpStartPos+i, qsrvIp[i]);

            }   
          for (int i = espIdStartPos; i < espIdStartPos+3; ++i) { EEPROM.write(i, 0); }
          for (int i = 0; i < qespId.length(); ++i)
            {
              EEPROM.write(espIdStartPos+i, qespId[i]);

            }            
          EEPROM.commit();
          content = "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}";
          statusCode = 200;
        } else {
          content = "{\"Error\":\"404 not found\"}";
          statusCode = 404;
          Serial.println("Sending 404");
        }
        server.send(statusCode, "application/json", content);
    });
  } else if (webtype == 0) {
    server.on("/", []() {
      IPAddress ip = WiFi.localIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      server.send(200, "application/json", "{\"IP\":\"" + ipStr + "\"}");
    });
    server.on("/cleareeprom", []() {
      content = "<!DOCTYPE HTML>\r\n<html>";
      content += "<p>Clearing the EEPROM</p></html>";
      server.send(200, "text/html", content);
      Serial.println("clearing eeprom");
      for (int i = 0; i < 96; ++i) { EEPROM.write(i, 0); }
      EEPROM.commit();
    });
    server.on("/setting", []() {
        String qsrvIp = server.arg("srvIp");
        String qespId = server.arg("espId");
        if (qsrvIp.length()>0) {
          //clear contents
          for (int i = srvIpStartPos; i < srvIpStartPos+15; ++i) { EEPROM.write(i, 0); }
          for (int i = 0; i < qsrvIp.length(); ++i)
            {
              EEPROM.write(srvIpStartPos+i, qsrvIp[i]);

            }   
          for (int i = espIdStartPos; i < espIdStartPos+3; ++i) { EEPROM.write(i, 0); }
          for (int i = 0; i < qespId.length(); ++i)
            {
              EEPROM.write(espIdStartPos+i, qespId[i]);
              Serial.print("Wrote: ");
              Serial.println(qespId[i]); 
            }
          EEPROM.commit();
          server.send(200, "application/json", "{\"Success\":\"new srv ip is saved to eeprom... \"}");
        } 
    }); 
    server.on("/whoareyou", [](){
        server.send(200, "application/json", "{\"espId\":"+espId+
                                           ", \"clientId\":"+clientId+
                                           ", \"clientVersion\":"+clientVer+"}");
    });
  }
}

void loop() {
  if (workingMode==1){
    server.handleClient();
  }else {
    server.handleClient();
    delay(5000);
  
    Serial.print("connecting to ");
    Serial.println(host);
    
    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 3000;
    client.stop();
    if (!client.connect(host, httpPort)) {
      Serial.println("connection failed");
      return;
    }
    
    // Get the defined led status
    String url = "/espStatus";
    
    //Send the keyPressed evt
    String pressedUrl = "/buttonIsPressed";
  
    //Serial.print("Requesting URL: ");
    //Serial.println(url);
    
    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" + 
                 "Connection: close\r\n\r\n");
    delay(10);
  
    String section="header";
    // Read all the lines of the reply from server and print them to Serial
    while(client.available()){
      String line = client.readStringUntil('\r');
      // Serial.print(line);
      // we’ll parse the HTML body here
      if (section=="header") { // headers..
        Serial.print(".");
        if (line=="\n") { // skips the empty space at the beginning 
          section="json";
        }
      }
      else if (section=="json") {  // print the good stuff
        section="ignore";
        String result = line.substring(1);
  
        // Parse JSON
        int size = result.length() + 1;
        char json[size];
        result.toCharArray(json, size);
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& json_parsed = jsonBuffer.parseObject(json);
        if (!json_parsed.success())
        {
          Serial.println("parseObject() failed");
          return;
        }
  
        // Make the decision to turn off or on the LED
        if (strcmp(json_parsed["status"], "0") == 0) {
          digitalWrite(pin, LOW); 
          Serial.println("led off");
        }
        else {
          digitalWrite(pin, HIGH);
          Serial.println("LED ON");
        }
  
      }
    }
    
    Serial.println();
    Serial.println("closing connection"); 
    
    if(digitalRead(inputPin) == HIGH){
        client.stop();
        if (!client.connect(host, httpPort)) {
            Serial.println("connection failed! I cannot send the button is pressed event");
            return;
        }
  
        //
        // Step 1: Reserve memory space
        //
        StaticJsonBuffer<200> jsonBuffer;
  
        //
        // Step 2: Build object tree in memory
        //
        JsonObject& root = jsonBuffer.createObject();
        root["btnPressed"] = "1";
        // This will send the button is pressed evt to the server
         client.print(String("GET ") + pressedUrl + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
         delay(10);
         Serial.println("The button was pressed create the json respone to "+ pressedUrl);
         while(client.available()){
              String line = client.readStringUntil('\r');
              //Serial.println("--"+ line);
          }
          
    }    
  }
  //delay(1000);
}
