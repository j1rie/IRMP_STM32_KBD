## Long key presses
If you are building your own kernel, you may patch it with hid_irmp.diff.  
Than long key presses will work perfectly, with repeat_timeout set to e.g. 130.  
Enable the module in Device drivers → HID support → Special HID drivers → IRMP USB-HID-keyboard support.  
It may be necessary to add 'rmmod hid_irmp', 'rmmod hid_generic' and 'modprobe hid_irmp' to /etc/init.d/boot.local (look for "irmp configured" in dmesg).

Without the patch your repeat_timeout must be set to 15 (default), and all keystrokes are new keys, so long key presses in kodi and acceleration will not work.

Instead of hid_irmp.diff you can try stm32kbd2uinput. Than long keystrokes work perfectly. Leave repeat_timeout at default (15), repeat_delay and repeat_period at 100 (or less).
