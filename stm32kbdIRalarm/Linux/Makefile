
PROJECTNAME=stm32kbdIRalarm
VERSION=0.0.8

CC              = g++
//PKG_CONFIG      = pkg-config

CFLAGS          = -O -Wall
//CFLAGS          = `$(PKG_CONFIG) --cflags libusb` -O -Wall
//LIBS            = `$(PKG_CONFIG) --libs libusb`

TMPDIR = /tmp


all: stm32kbdIRalarm

.c.o:
	$(CC) $(CFLAGS) -c $<

stm32kbdIRalarm: stm32kbdIRalarm.o
	$(CC) -o stm32kbdIRalarm stm32kbdIRalarm.o

#$(LIBS)

clean:
	rm -f stm32kbdIRalarm.o
	rm -f stm32kbdIRalarm

dist: clean
	@-rm -rf $(TMPDIR)/$(PROJECTNAME)-$(VERSION)
	@mkdir $(TMPDIR)/$(PROJECTNAME)-$(VERSION)
	@cp -a * $(TMPDIR)/$(PROJECTNAME)-$(VERSION)
	@tar cjf $(PROJECTNAME)-$(VERSION).tar.bz2 -C $(TMPDIR) $(PROJECTNAME)-$(VERSION)
	@-rm -rf $(TMPDIR)/$(PROJECTNAME)-$(VERSION)
	@echo Distribution package created as $(PROJECTNAME)-$(VERSION).tar.bz2
