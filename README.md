# Utility Shaft Monitoring System

An embedded distributed monitoring system for underground utility shafts based on PIC microcontrollers. The project consists of one master node with Ethernet connectivity and multiple slave nodes communicating over an RS-485 bus.

The system periodically collects status information from all slave devices, displays the current system state on an LCD, and provides monitoring through an embedded HTTP server.

> **Note:** During development, water and manhole cover events were simulated using digital inputs (push buttons/switches) on the development boards.

---

## System Architecture

The system is organized as a master-slave network.

### Master Node

- Polls slave devices over RS-485
- Collects monitoring data
- Displays system status on an LCD
- Serves monitoring information through Ethernet using the ENC28J60 controller
- Supports monitoring of up to 32 slave nodes

### Slave Node

Each slave node:

- Receives commands from the master
- Monitors digital inputs that simulate:
  - Water detection
  - Manhole cover status
- Generates alarm conditions
- Sends its status back to the master
- Displays local information on an LCD

---

## Features

- Distributed embedded architecture
- RS-485 master-slave communication
- Ethernet connectivity
- Embedded HTTP server
- LCD status display
- Monitoring of up to 32 slave nodes
- Alarm reporting
- Digital input simulation of water and manhole cover events

---

## Hardware

### Master

- PIC18F2525
- ENC28J60 Ethernet Controller
- LCD Display
- RS-485 Interface

### Slave

- PIC16F877A
- LCD Display
- RS-485 Interface
- Digital inputs used to simulate sensor events

---

## Communication

- RS-485
- UART
- SPI
- Ethernet

---

## Software

- mikroC PRO for PIC
- Embedded C

---

## Repository Structure

```text
firmware/
├── master/
│   └── MasterSahte.c
└── slave/
    └── SlaveSahte.c
```

---

## How It Works

1. The master periodically polls every slave device over the RS-485 bus.
2. Each slave reads its digital inputs representing simulated sensor states.
3. The slave returns its current status and alarm information.
4. The master updates the LCD display and serves the collected information through the embedded HTTP server.

---

## Future Improvements

- Replace simulated inputs with real sensors
- Event logging
- RTC support for timestamps
- Sensor diagnostics
- Modular firmware organization

---

## Author

**Dušan Pejčić**

Electrical Engineering Student

Interested in Embedded Systems, Firmware Development and Industrial Electronics.
