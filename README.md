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
* bootloader for comfortable firmware update

## Software Linux
* GUI config-tool stm32kbdIRconfig_gui for configuration: set, get and reset wakeups, irdata, keys, alarm time, repeat. It is possible to program wakeups and irdata via remote control. Create the eeprom map with remote control and mouse. Comprehensive debug messages.
* command line program stm32kbdIRconfig for configuration: set, get and reset wakeups, irdata, keys, alarm time, repeat. It is possible to program wakeups and irdata via remote control.
* stm32kbdIRalarm for setting and reading alarm time via script

## Software Windows
* GUI config-tool stm32kbdIRconfig_gui (same as linux)
* command line programm stm32kbdIRconfig (same as linux)
* stm32kbdIRalarm for setting and reading alarm time via script

## Creating an eeprom map
The stm32kbdIRconfig_gui can be used for building an eeprom map with the remote control.  
There are two methods.  
*.jpg  
First method: Open the template keymap.  
Click the line with the key's name, click "set by remote - irdata", press the button on the remote - repeat this for all buttons.

*.jpg  
Second method: Click a line, click "set by remote - irdata", press the button on the remote.  
Click a line, enter key (and modifier), click "set - key".  
Repeat this as often as needed.  

You can test the keymap immediately with evtest: After pressing the button on the remote, evtest should show the corresponding key.
At the end you may save the eeprom map as a backup.

## Lerning wakeup
If the wakeup is empty, the first received IR data will be stored into the wakeup.  
Change wakeup: press set by remote - wakeup, press button on remote control, finished.  
Important for testing: wakeup happens only, if the PC is powered off.  
If the PC is powered on, only the key is send to the PC (so you can use the same button for power on and power off).  

## Building from source
The sources are complete only after running the prepare script, the script downloads the ST- and IRMP-sources, extracts and patches them.  
This makes it possible to put the patches under GPL, without interfering with the original licences.  
It is easier to see, what has been changed, too.  
Compile with arm-none-eabi-gcc.  

## Pin-Out
See config.h.

## eventlircd, udev and systemd
See /eventlircd/README.

## Thanks to
Frank Meyer for IRMP. [1]  
Uwe Becker for adapting IRMP to the STM32F4xx and his USB-HID for the STM32F4xx. [2]  
His work helped me in getting started with the STM32F105.  
Andrew Kambaroff for his USB-HID for the STM32F103. [3]  
His work helped me in getting started with the STM32F103.  
Ole Ernst for code review, linux Makefile and linux download-extract-patch-script, much better macro implementation and new protocol. [4]  
Manuel Reimer for gcc-4.9 fix, SimpleCircuit and check if host running. [5]  

[1] https://www.mikrocontroller.net/articles/IRMP  
[2] http://mikrocontroller.bplaced.net/wordpress/?page_id=744  
[3] http://sysmagazine.com/posts/208026/  
[4] https://github.com/olebowle  
[5] https://github.com/M-Reimer  

## Discussion
For comments, questions, suggestions go to https://www.vdr-portal.de/forum/index.php?thread/123572-irmp-auf-stm32-ein-usb-ir-empf%C3%A4nger-sender-einschalter-mit-wakeup-timer/ .

## Find more information on
https://www.mikrocontroller.net/articles/IRMP_auf_STM32_-_ein_USB_IR_Empf%C3%A4nger/Sender/Einschalter_mit_Wakeup-Timer

Much fun with IRMP on STM32!
