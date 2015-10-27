# appBoot Application

appBoot is a demo application for setting and storing an AP ssid and password to anesp8266 chipset when the last used AP is unavailable. If connection to the external AP is succesfull then esp initiliazes a small application that reads a json request, and sends a json response to a web server. The demo is based on https://gist.github.com/dogrocker/f998dde4dbac923c47c1 application.

The esp client is associated with an espId which allows a server to send a command to a specific client.

The esp client has also a clientId that corresponds to the functionality that is supported by the client. It also has a clientVersion that indicates the functionality version, so that a server can identify the commands supported by a given client.
# use
1.Locate and connect to the network named 'test'.
Use 192.168.4.1/setting?ssid=xxxx&pass=jkd&&srcIp=serverIp&espId=agivenid to save external AP credentials, configure the ip of the  server to connect and assign an espId to a chipset.

2.Get esp client info
use <esp ip>/whoareyou to get a JSON object that describes the esp client

3.Change an esp client parameters
use <esp ip>/setting?srcIp=serverIp&espId=agivenid to  configure the ip of the  server to connect and assign an espId to a chipset.

4.Clear old AP credentials

use <esp ip>/cleareeprom to clear stored SSID and password
