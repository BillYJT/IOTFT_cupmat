# IOTFT_cupmat

This repository contains the design files of a smart ESP-enabled cupmat. This may be the start of a series where the most unneccesary things get 'IoTized'. 

## Hardware
The hardware design essentially consists of an ESP-12, a pressure sensor and a battery management chip. The system design is largely based off the [NodeMCU DEVKIT V1.0](https://github.com/nodemcu/nodemcu-devkit-v1.0)

## Software
The current implementation has very limited functions and is only for testing purposes. Thanks to the [WIFIManager library by tzapu](https://github.com/tzapu/WiFiManager), the ESP can spin up 
a DNS and WebServer, where user can submit the login details of their wireless network. The ESP can then login to the network and publish data via a webserver on the local network.

## TODO
* Enclosure design
* More robust software
* Going battery-less
