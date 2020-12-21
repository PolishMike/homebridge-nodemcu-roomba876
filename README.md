## This project is to add iRobot roomba 876 (or others without smart/WiFi capabilities) controlls over Apple Home.

[![npm version](https://badge.fury.io/js/homebridge-nodemcu-roomba876.svg)](https://badge.fury.io/js/homebridge-nodemcu-roomba876)

homebridge-nodemcu-roomba876
=========

This Homebridge plugin adds switch, battery level and charge status intrefaces via REAT API calls underneath.

API calls go to nodemcu (esp8266) WiFi microcontroller connected to iRobot roomba to read its status and control it.

See Hardware requirements to make it work for you and your roomba.

## Credits

This code was built upon the work in the following projects:

* [homebridge](https://github.com/nfarina/homebridge)
* [homebridge-roomba690](https://github.com/gbro115/homebridge-roomba690)

## Prerequisities

* iRobot romba without WiFi capabilities (otherwise, you don't need nodemcu and extra code), my model is 876 but most of robots are having the same open interface (OI) spec so should work well
* Raspberry Pi or any other Mac/PC to host [homebridge](https://github.com/nfarina/homebridge) software
* Nodemcu microcontroller to run HTTP REST API server and proxy homebrdge commands to roomba

## Features

* Start/stop Roomba `Hey Siri, start Roomba*
* Check on/off status *Hey Siri, is the Roomba on?*
* Check charging status *Hey Siri, is the Roomba charging?*
* Check battery status *Hey Siri, is the Roomba charged?*

## Installation

`npm install -g homebridge-nodemcu-roomba876`

## Homebridge configuration

Add to your config.json's accessory section (add only *accessories* entry, don't copy whole code shippet):

```
{
    "bridge": {
		"name": "Homebridge",
		"username": "CD:22:3D:E3:CE:30",
		"port": 51826,
		"pin": "123-45-678"
    },
    "description": "My Homebridge",
    "platforms": [],
    "accessories": [
		{
			"accessory": "Roomba876",
			"model":"876",
			"name": "Roomba",
			"hostname": "http://mdns-host-or-ip-address-of-your-roomba-nodemcu",
		}
    ]
}
```

The Model and Name options can be set to whatever you'd like.
Hostname is an IP address of nodemcu's microcontroller or its mDNS host name used

## Nodemcu

This is separate hardware to listen on host/IP with REST interface accepting API calls from this homebridge plugin.

It should be developed and flashed separately from this plugin. You can use Arduino IDE to develop and flash it.

It passes commands from homebridge/Siri to roomba over open interface port (OI).

### Source

Source is located in `nodemcu` folder of this project. But rest of the routine is up to you.

### Wiring

Connections from Roomba to Wemos D1 mini:  
Soldered directly onto Roomba motherboard.   
Roomba Gnd and Batt terminals are located near the center of the mainboard.  
Feeding those through a buck converter to Wemos GND and 3.3V pins.  
  
Roomba serial pin 5 - soldered to C259 - D3 on Wemos (Device Detect pin)  
Roomba serial pin 3 - soldered on C258 - TX on Wemos (Roomba RXD)  
Roomba serial pin 4 - soldered on C261 - RX on Wemox (Roomba TXD)  

Flasing the Wemos with Arduino kept faining until I set the flash baud rate to 115200.   


