
PROJECTNAME=stm32kbdstatusled
VERSION=0.0.1

CC              = g++
//PKG_CONFIG      = pkg-config

CFLAGS          = -O -Wall
//CFLAGS          = `$(PKG_CONFIG) --cflags libusb` -O -Wall
//LIBS            = `$(PKG_CONFIG) --libs libusb`

TMPDIR = /tmp


all: stm32kbdstatusled

.c.o:
	$(CC) $(CFLAGS) -c $<

stm32kbdstatusled: stm32kbdstatusled.o
	$(CC) -o stm32kbdstatusled stm32kbdstatusled.o

#$(LIBS)

clean:
	rm -f stm32kbdstatusled.o
	rm -f stm32kbdstatusled

dist: clean
	@-rm -rf $(TMPDIR)/$(PROJECTNAME)-$(VERSION)
	@mkdir $(TMPDIR)/$(PROJECTNAME)-$(VERSION)
	@cp -a * $(TMPDIR)/$(PROJECTNAME)-$(VERSION)
	@tar cjf $(PROJECTNAME)-$(VERSION).tar.bz2 -C $(TMPDIR) $(PROJECTNAME)-$(VERSION)
	@-rm -rf $(TMPDIR)/$(PROJECTNAME)-$(VERSION)
	@echo Distribution package created as $(PROJECTNAME)-$(VERSION).tar.bz2
