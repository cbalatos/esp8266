# appBoot Application

appBoot is a demo application for setting and storing an AP ssid and password when the last used AP is unavailable. If connection to the external AP is succesfull then esp initiliazes a small application that reads a json request, and sends a json response to a web server. The demo is based on https://gist.github.com/dogrocker/f998dde4dbac923c47c1 application.

# use
Locate and connect to the network named 'test'.
Use 192.168.4.1/setting?ssid=xxxx&pass=jkd to save external AP credentials.


