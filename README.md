# Final Project: Smart Crowd Detection System
The final project is design to use an IR sensor, ultrasonic sensor, MSP430F5529, and ESP8266 WiFi Module to 
detect the amount of people in an area when they trigger the sensors. The MSP430 transmits the data onto to the WiFi module 
and uses MQTT protocol to deliever the information onto a MQTT broker or cloud server. By establishng a connection through the 
MQTT broker, anyone will be able to subscribe to the topic and be able to see the real time data of the counter.     

![alt text](https://github.com/RU09342-F18/intro-to-embedded-final-project-convertoboxers_final/blob/master/ImageOfSystem.JPG)

# Authurs: Jan Garcia and Anwar Hussein 

# Components
Here are some of the required components used in this project:

1. MSP430F5529 Microcontroller from Texas Instruments(TI)
2. GP2Y0A21YK0F: Sharp IR Sensor
3. ESP8266: WiFi Module
4. Parallax 28015-ND: Ultrasonic Sensor
5. Code Composer Studio Latest Version

# Getting Started
## Prerequisites
Important libraries required to run the code:
```
#include <msp430.h>
#include <intrinsics.h>
#include <stdint.h>
```

## Functionality of the Code
The MSP430F5529 utilizes the 12-bit ADC for cacluating the distance of the sensors and UART, as an optionally supoort to see the data on Realterm.
The main.c contains 3 functions:
```
setUart();
setADC();
sonic();
```
The setUart() and setADC() functions are obvious on what they do. The sonic() function is configured to calculated the distance and increment the counter when it reaches the maximum threshold of the signal. It utilizes the timerA0 peripheral to set a specific time at which the clock will capture and set for each echco sound the ultrasonic sensor recieves and calculates the distance using the MSP430. 
