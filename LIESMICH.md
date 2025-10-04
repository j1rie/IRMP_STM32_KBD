# IRMP auf STM32 - ein USB-HID-Keyboard IR-Empfänger/Sender/Einschalter mit Wake-up Timer

Ein Fernbedienungsempfänger mit vielen Funktionen basierend auf billiger Hardware.

![Stick](https://raw.githubusercontent.com/wiki/j1rie/IRMP_STM32_KBD/images/Stick.jpg)

![Stick am Board](https://raw.githubusercontent.com/wiki/j1rie/IRMP_STM32_KBD/images/Stick_am_Board.jpg)

## Einleitung
Für 2 € kann man bei ebay einen ST-Link Emulator oder ein STM32F103-Entwicklungsboard, meist aus chinesischer Produktion, kaufen. Auf den STM32F103 Mikrocontroller darauf wird eine Open-Source-Firmware mit vielen Funktionen geflasht.

## Merkmale
* Anschluss über USB 
* registriert sich als USB HID Keyboard Gerät *und* erscheint als /dev/hidraw oder „Custom HID device“, keine Treiber erforderlich
* überträgt die Daten über USB2 in Hardware mit Fullspeed 
* IR-Empfänger (ca. 40 Protokolle in Hardware dekodiert) 
* Einschalten des PCs per Fernbedienung von S3 (STR) und S4 (STD) über USB oder von S3, S4 und S5 (SoftOff) über den Einschalter  auf der Hauptplatine (+5V erforderlich, über USB oder vom Netzteil oder der Hauptplatine) 
* Einschalten des PCs über den eingebauten Timer von S3 (STR) und S4 (STD) über USB oder von S3, S4 und S5 (SoftOff) über den Einschalter auf der Hauptplatine (+5V erforderlich, über USB oder vom Netzteil oder der Hauptplatine) 
* IR-Sender (etwa 40 Protokolle) 
* die Konfiguration wird im emulierten Eeprom gespeichert
* Bootloader für einfaches Firmware Update
* Makro Unterstützung (*)

Mit einem Makro kann eine Folge von IR-Befehlen über die Sendediode gesendet werden, wenn ein Trigger empfangen wird.
macroslot0 ist der Auslöser, macroslot1 ... macroslot7 sind die zu sendenden Befehle. 

(*) Nur RP2xxx

## Software Linux
* GUI-Konfigurationswerkzeug stm32kbdIRconfig_gui für die Konfiguration: Wakeups, irdata, Tasten, Alarmzeit, Wiederholung setzen, abrufen und zurücksetzen. Es ist möglich, Wakeups und irdata über Fernsteuerung zu programmieren. Erstellen der Eeprom-Map mit Fernbedienung und Maus. Umfassende Debug-Meldungen. Firmware-Aktualisierung.
* Interaktives Kommandozeilenprogramm stm32kbdIRconfig zur Konfiguration: Setzen, Abrufen und Zurücksetzen von Wakeups, Makros, irdata, Tasten, Alarmzeit, Wiederholung und Senden von IR. Es ist möglich, Wakeups, Makros und irdata per Fernsteuerung zu programmieren.
* stm32kbdIRalarm zum Setzen und Auslesen der Alarmzeit per Skript
* stm32FWupgrade für das Firmware Upgrade per Kommandozeile

## Software Windows
* GUI-Konfigurationsprogramm stm32kbdIRconfig_gui (wie bei Linux)
* interaktives Kommandozeilenprogramm stm32kbdIRconfig (wie bei Linux)
* stm32kbdIRalarm zum Setzen und Auslesen der Alarmzeit per Skript
* stm32FWupgrade für das Firmware-Upgrade per Kommandozeile

## Erstellen einer Eeprom-Map
Das stm32kbdIRconfig_gui kann verwendet werden, um eine Eeprom-Map mit der Tastatur und der Fernbedienung zu erstellen.  
Es gibt drei Methoden.  
  
(1)  
![Tastatur + irdata mode](https://raw.githubusercontent.com/wiki/j1rie/IRMP_STM32_KBD/images/keyboard+irdata_mode.jpg)  
Erste Methode: Klicken Sie auf eine Zeile, klicken Sie auf „set by remote - keyboard + irdata“, drücken Sie den Modifikator oder die Taste auf Ihrer Tastatur, drücken Sie die Taste, wenn der erste ein Modifikator war, drücken Sie den Knopf auf der Fernbedienung.  
Der rote Text unter der Schaltfläche „keyboard + irdata“ führt Sie durch den Vorgang.  
Wiederholen Sie diesen Vorgang so oft wie nötig.  
  
(2)  
![Einstellen von irdata in die Vorlagen keymap](https://raw.githubusercontent.com/wiki/j1rie/IRMP_STM32_KBD/images/setting_irdata_into_template_map.jpg)  
Zweite Methode: Öffnen Sie die Vorlage keymap. Entfernen Sie Zeilen mit Tasten, die Sie nicht benötigen.  
Klicken Sie auf die Zeile mit dem Namen der Taste, klicken Sie auf „set by remote - irdata“, drücken Sie die Taste auf der Fernbedienung - wiederholen Sie dies für alle Tasten.  
Klicken Sie auf „flash eeprom“.  
  
(3)  
![building eeprom map](https://raw.githubusercontent.com/wiki/j1rie/IRMP_STM32_KBD/images/building_eeprom_map.jpg)  
Dritte Methode: Klicken Sie auf eine Zeile, klicken Sie auf „set by remote - irdata“, drücken Sie die Taste auf der Fernbedienung.  
Klicken Sie auf eine Zeile, geben Sie den Key (und den Modifikator) ein und klicken Sie auf „set - key“.  
Wiederholen Sie diesen Vorgang so oft wie nötig.  

Wenn Sie die Tastatur + irdata-Taste zweimal drücken, können Sie Ihre Tastaturtasten und unter Windows sogar bereits konfigurierte Tasten auf der Fernbedienung testen.  

Unter Linux können Sie die Eeprom-Map mit evtest testen: Nachdem man die Taste auf der Fernbedienung gedrückt hat, sollte evtest die entsprechende Taste anzeigen.

Es wird empfohlen, mit einem frischen Eeprom zu beginnen, indem man „eeprom map - reset eeprom“ drückt.

## Wakeup anlernen
Wenn das Wakeup leer ist, werden die ersten empfangenen IR-Daten im Wakeup gespeichert.  
Wakeup mit stm32kbdIRconfig_gui ändern: set by remote - wakeup drücken, Taste auf der Fernbedienung drücken.  
Wichtig zum Testen: Wakeup findet nur statt, wenn der PC ausgeschaltet ist.  
Wenn der PC eingeschaltet ist, wird nur die Taste an den PC gesendet (man kann also die gleiche Taste zum Ein- und Ausschalten verwenden).  

## Aus den Quellen bauen
Die Quellen sind erst nach dem Ausführen des Prepare-Skripts vollständig, das Skript lädt die IRMP-Quellen herunter, extrahiert und patcht die Quellen. Die ST-Quellen müssen Sie manuell herunterladen, da Sie sich leider registrieren müssen.  
Dadurch ist es möglich, die Patches unter die GPL zu stellen, ohne in die Originallizenzen einzugreifen.  
Es ist auch einfacher zu sehen, was geändert wurde.  
Kompilieren mit arm-none-eabi-gcc.  

## Pin-Out
Siehe /STM32Fxxxxx/src/config.h.

## Flashen des Bootloaders mit einem ST-Link
Holen Sie sich das STM32 ST-Link Dienstprogramm.  
Verbinden Sie  
Programmiergerät SWDIO -> IO  
Programmiergerät SWCLK -> CLK  
Programmiergerät GND -> G  
Programmiergerät 3V3 -> V3  
File - Open File - boot.blackDev.bin (https://github.com/j1rie/STM32F103-bootloader/tree/master/binaries)
Target - Program & Verify (bestätigen Sie die Meldung über den Ausleseschutz, falls sie erscheint)  
https://github.com/j1rie/IRMP_STM32_KBD/wiki/Herstellungsprozess-Schwarze#bootloader-flashen  

## Flashen der Firmware
Trennen Sie die Verbindung zum USB.  
Linux: Start ./stm32FWupgrade ./2019-07-06_16-32_blackDev_BL_SC_jrie.bin  
Windows: Start stm32FWupgrade.exe 2020-02-15_00-29_blueDev_BL_SC_jrie.bin  
An den USB anschließen.  
Anstelle von stm32FWupgrade können Sie auch stm32IRconfig_gui verwenden.  
https://github.com/j1rie/IRMP_STM32_KBD/wiki/Herstellungsprozess-Schwarze#firmware-flashen  

## Andere Möglichkeiten zum Flashen
Flashen mit einem USB-serial-TTL ist auf den Entwicklerboards möglich.  
Flashen über USB ist auf den Boards möglich, die es unterstützen.  
Siehe https://www.mikrocontroller.net/articles/IRMP_auf_STM32_-_ein_USB_IR_Empf%C3%A4nger/Sender/Einschalter_mit_Wakeup-Timer#Firmware_Flashing  

## eventlircd, udev und systemd
Siehe /eventlircd/README.

## Dank an
Frank Meyer für IRMP. [1]  
Uwe Becker für die Anpassung von IRMP an den STM32F4xx und sein USB-HID für den STM32F4xx. [2]  
Seine Arbeit hat mir beim Einstieg in den STM32F105 geholfen.  
Andrew Kambaroff für sein USB-HID für den STM32F103. [3]  
Seine Arbeit half mir bei den ersten Schritten mit dem STM32F103.  
Ole Ernst für die Code-Überprüfung, das Linux-Makefile und das Linux-Download-Extract-Patch-Skript und das neue Protokoll. [4]  
Manuel Reimer für den gcc-4.9 Fix, SimpleCircuit und die Überprüfung, ob der Host läuft. [5]  
Martin Neubauer für die Integration in EasyVDR, Entwicklung einer Zusatzplatine und den Verkauf von fertigen Empfängern mit Kabeln. [6]  
Alexander Grothe für die Integration in yaVDR und für wertvolle Hilfe bei schwierigen Fragestellungen. [7]  
Helmut Emmerich für den Verkauf von fertigen Empfängern mit Kabeln und die Unterstützung der Anwender. [8]  
Claus Muus für die Integration in MLD. [9]  
Bernd Juraschek für vdr-plugin-statusleds, das die Basis war für vdr-plugin-statusleds2irmphidkbd. [10]  
Alle Benutzer für Fragen, Feature-Anfragen, Feedback und Ideen.  

[1] https://www.mikrocontroller.net/articles/IRMP  
[2] http://mikrocontroller.bplaced.net/wordpress/?page_id=744  
[3] https://habr.com/post/208026/  
[4] https://github.com/olebowle  
[5] https://github.com/M-Reimer  
[6] https://www.vdr-portal.de/user/4786-ranseyer/, https://github.com/ranseyer/STM32-IRMP-Hardware  
[7] https://www.vdr-portal.de/user/24681-seahawk1986/  
[8] https://www.vdr-portal.de/user/13499-emma53/  
[9] https://www.vdr-portal.de/user/942-clausmuus/  
[10] <statusleds@bjusystems.de>  

## Diskussion und weitere Informationen
Kommentare, Fragen und Anregungen können an https://www.vdr-portal.de/forum/index.php?thread/132289-irmp-auf-stm32-ein-usb-hid-keyboard-ir-empf%C3%A4nger-einschalter-mit-wakeup-timer/ gesendet werden.

## Bauanleitungen
https://github.com/j1rie/IRMP_STM32_KBD/wiki/Herstellungsprozess-Schwarze  
https://www.mikrocontroller.net/articles/IRMP_auf_STM32_%E2%80%93_stark_vereinfachte_Bauanleitung  
https://www.mikrocontroller.net/articles/IRMP_auf_STM32_-_Bauanleitung  
Die letzten beiden sind auch in /doc/old/Deutsch.  

Viel Spaß mit IRMP auf STM32!

Copyright (C) 2018-2025 Jörg Riechardt

