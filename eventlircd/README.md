## Automatical start and stop of eventlircd at boot or on device dis/reconnect
put the udev rule '70-irmp.rules' in your udev rules directory (/etc/udev/rules.d/) (depending on your kernel),  
put the udev rule '98-eventlircd.rules' in your udev rules directory (/etc/udev/rules.d/),  
put the evmap '03_1209_4445.evmap' in your evmap directory (/usr/etc/eventlircd.d/),  
put the systemd service 'eventlircd.service' in your services directory (/etc/systemd/system/),  
put the systemd service 'eventlircd.socket' in your services directory (/etc/systemd/system/),  
put the systemd-tmpfiles configuration file 'eventlircd.conf' into your tmpfiles directory (/etc/tmpfiles.d/),  
and run "systemctl enable eventlircd.service eventlircd.socket" once.  
'03_1209_4445.evmap' will work for yaVDR.

## Was the computer started by the receiver?
You can log when the receiver has started the computer.  
At each start by the receiver, it sends KEY_REFRESH every second for a configurable period of time. The first one is written to the log file /var/log/started_by_IRMP_STM32_KBD. To do this, log_KEY_REFRESH.sh is called by irexec or triggerhappy.  
If an entry appears in the log file (/var/log/started_by_IRMP_STM32_KBD) shortly after the boot messages (depending on the distribution/var/log/boot.msg or similar), you know that the computer was started by the receiver.  
If the entry in the log file is older than the boot messages, it was started by switching on the computer or by timer.

This is handy if you have a remote control with macros like the Logitech and you want to use one button to switch on not only VDR but also other devices. If the VDR was started by a timer, it would otherwise go off when you want to switch on all other devices. This can be avoided by a query in the shutdown script (see the example script vdrshutdown).  
log_KEY_REFRESH.sh is called by i.e. triggerhappy or irexec.  
For triggerhappy put irmp_stm32_kbd.conf into /etc/triggerhappy/triggers.d/  
stm32kbd2uinput writes into /var/log/started_by_IRMP_STM32_KBD directly.

## Without eventlircd: Finding keysym's
Softhddevice passes X11 keypresses on to VDR as 'XKeySym'. To find them, start xev in an xterm with the focus on the test window.  
When you then press a button on the remote, you'll see the corresponding keysym.  
For example a button is configured as 'KEY_I', which gives the keysym 'i', which is mapped to 'Info'. See kbd.map and remote.conf.

## Without eventlircd: Resuming softhddevice
No X11 keypresses are passed on by softhddevice when suspended and SuspendClose=1.  
E.g. triggerhappy is needed to resume from suspend. See irmp_stm32_kbd.conf and 70-irmp.rules.

## Don't use softhddevice for remote control
It is recommended to use vdr-plugin-irmphidkbd instead, because softhddevice's remote function is not as precise.
