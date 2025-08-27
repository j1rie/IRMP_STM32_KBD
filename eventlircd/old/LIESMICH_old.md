## Lange Tastendrücke
Wenn man einen eigenen Kernel baut, kann man ihn mit hid_irmp.diff patchen.  
Dann funktionieren die langen Tastendrücke perfekt, wenn man repeat_timeout auf  z.B. 130 setzt.  
Man aktiviert das Modul unter Device drivers → HID support → Special HID drivers → IRMP USB-HID-keyboard support.  
Es kann sein, dass man "rmmod hid_irmp", "rmmod hid_generic" und "modprobe hid_irmp" zu /etc/init.d/boot.local hinzufügen muss (taucht in dmesg "irmp configured" auf?).

Ohne den Patch muss "repeat_timeout" auf 15 gesetzt werden (default). Dann sind alle Tastenanschläge neue Tasten und lange Tastendrücke in Kodi und Beschleunigung funktionieren deshalb nicht.

Statt hid_irmp.diff kann man stm32kbd2uinput nehmen. Damit funktionieren lange Tastendrücke perfekt. Dabei bleibt repeat_timeout auf default (15), repeat_delay und repeat_period auf 100 (oder weniger).
