## This project is to add iRobot roomba 876 controlls (with no WiFi and native app) to Apple Home.

homebridge-nodemcu-roomba876
=========

This Homebridge plugin adds switch, battery level and charge status intrefaces with REAT API calls underneath.
API calls go to nodemcu (esp8266) WiFi microcontroller connected to iRobot roomba to read its status and control it.

## Credits

This code was built upon the work in the following projects:

* [homebridge](https://github.com/nfarina/homebridge)
* [homebridge-roomba690](https://github.com/gbro115/homebridge-roomba690)

## Features

* Start / stop Roomba *Hey Siri, turn on the Roomba*
* Check on/off status *Hey Siri, is the Roomba on?*
* Check charging status *Hey Siri, is the Roomba charging?*
* Check battery status *Hey Siri, is the Roomba charged?*

## Installation

`npm install -g homebridge-roomba690`

## Homebridge configuration

Add to your config.json's accessory section:

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
	    "hostname": "ip-address-of-your-nodemcu",
	}
    ]
}
```

The Model and Name options can be set to whatever you'd like.

TBD...
