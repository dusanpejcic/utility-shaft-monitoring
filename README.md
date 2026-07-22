# Utility Shaft Monitoring System

Embedded monitoring system for underground utility shafts using PIC microcontrollers. The system consists of a master node with Ethernet connectivity and multiple slave nodes communicating over RS-485 for real-time monitoring and alarm reporting.

## Features

- Monitoring of manhole cover status
- Water level detection
- RS-485 master-slave communication
- Ethernet monitoring using ENC28J60
- Embedded HTTP server
- LCD status display
- Alarm indication
- Support for multiple utility shafts

## Hardware

### Master
- PIC18F2525
- ENC28J60 Ethernet Controller
- LCD Display

### Slave
- PIC16F877A
- Water Sensor
- Manhole Cover Sensor
- LCD Display

## Communication

- RS-485
- SPI
- Ethernet

## Development Environment

- mikroC PRO for PIC
- Embedded C

## Author

Dušan Pejčić
