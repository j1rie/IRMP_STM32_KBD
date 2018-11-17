# IRMP on STM32 - a USB Keyboard IR receiver + powerswitch with wakeup-timer

A remote control receiver with many functions based on cheap hardware.
 
## Introduction
For 2 - 4 € you can buy on ebay a ST-Link emulator or a STM32F103 development board, manufactured in China usually. Onto the STM32F103 mikrocontroller on it an open source firmware with many functions will be flashed. 

## Functions
* connection via USB 
* registers as USB HID Keyboard device *and* appears as /dev/hidraw or "Custom HID device", no drivers needed
* transfers the data via USB2 in hardware with fullspeed 
* IR receiver (ca. 40 protocols decoded in hardware) 
* switch PC on via remote control from S3 (STR) and S4 (STD) via USB or from S3, S4 and S5 (SoftOff) via motherboard-switch (+5V needed, on USB or from PSU or motherboard) 
* switch PC on via build in timer from S3 (STR) and S4 (STD) via USB or from S3, S4 and S5 (SoftOff) via motherboard-switch (+5V needed, on USB or from PSU or motherboard) 
* the configuration is stored in the emulated eeprom 

## Find more information on
https://www.mikrocontroller.net/articles/IRMP_auf_STM32_-_ein_USB_IR_Empf%C3%A4nger/Sender/Einschalter_mit_Wakeup-Timer

