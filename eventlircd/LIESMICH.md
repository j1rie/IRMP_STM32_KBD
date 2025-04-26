## Automatisches starten und stoppen von eventlircd beim Booten oder beim Anschliessen/Entfernen des Geräts
Die udev Regel '70-irmp.rules' wird in das udev rules Verzeichnis (/etc/udev/rules.d/) (abhängig vom kernel) kopiert,  
die udev Regel '98-eventlircd.rules' wird in das udev rules Verzeichnis (/etc/udev/rules.d/) kopiert,  
die evmap '03_1209_4445.evmap' wird in das evmap Verzeichnis (/usr/etc/eventlircd.d/) kopiert,  
der systemd service 'eventlircd.service' wird in das services Verzeichnis (/etc/systemd/system/) kopiert,  
der systemd service 'eventlircd.socket' wird in das services Verzeichnis (/etc/systemd/system/) kopiert,  
die systemd-tmpfiles Konfigurationsdatei 'eventlircd.conf' wird in das tmpfiles Verzeichnis (/etc/tmpfiles.d/) kopiert,  
und es wird "systemctl enable eventlircd.service eventlircd.socket" einmal ausgeführt.  
'03_1209_4445.evmap' funktioniert unter yaVDR.

## Lange Tastendrücke
Wenn man einen eigenen Kernel baut, kann man ihn mit hid_irmp.diff patchen.  
Dann funktionieren die langen Tastendrücke perfekt, wenn man repeat_timeout auf  z.B. 130 setzt.  
Man aktiviert das Modul unter Device drivers → HID support → Special HID drivers → IRMP USB-HID-keyboard support.  
Es kann sein, dass man "rmmod hid_irmp", "rmmod hid_generic" und "modprobe hid_irmp" zu /etc/init.d/boot.local hinzufügen muss (taucht in dmesg "irmp configured" auf?).

Ohne den Patch muss "repeat_timeout" auf 15 gesetzt werden (default). Dann sind alle Tastenanschläge neue Tasten und lange Tastendrücke in Kodi und Beschleunigung funktionieren deshalb nicht.

Statt hid_irmp.diff kann man stm32kbd2uinput nehmen. Damit funktionieren lange Tastendrücke perfekt. Dabei bleibt repeat_timeout auf default (15), repeat_delay und repeat_period auf 100 (oder weniger).

## Wurde der Computer vom Empfänger gestartet?
Man kann protokollieren, wann der Empfänger den Computer gestartet hat.  
Bei jedem Start durch den Empfänger sendet er eine konfigurierbare Zeit lang sekündlich KEY_REFRESH. Der erste wird in die Logdatei /var/log/started_by_IRMP_STM32_KBD geschrieben. Dazu wird von irexec oder triggerhappy log_KEY_REFRESH.sh aufgerufen.  
Wenn kurz nach den Bootmeldungen (je nach Distribution/var/log/boot.msg o.ä.) ein Eintrag im Logfile (/var/log/started_by_IRMP_STM32_KBD) landet, weiß man, dass der Computer vom Empfänger gestartet wurde.  
Wenn der Eintrag im Logfile älter ist als die Bootmeldungen, wurde per Einschalter am Computer oder per Timer gestartet.

Das ist nützlich, wenn eine Logitech Fernbedienung mit Makros auf einen Tastendruck hin den VDR und alle anderen Geräte einschaltet. Wenn der VDR durch einen Timer gestartet wurde, würde er sonst aus gehen, wenn man alle anderen Geräte einschalten will. Durch eine Abfrage im shutdown-Skript kann man das vermeiden (siehe das Beispielskript vdrshutdown).  
log_KEY_REFRESH.sh wird z.B. von triggerhappy oder irexec aufgerufen.  
Für triggerhappy wird irmp_stm32_kbd.conf nach /etc/triggerhappy/triggers.d/ kopiert  
stm32kbd2uinput schreibt direkt in /var/log/started_by_IRMP_STM32_KBD.
