
PROJECTNAME=stm32kbdIRconfig
VERSION=0.0.16

CC              = g++
//PKG_CONFIG      = pkg-config

CFLAGS          = -O -Wall
//CFLAGS          = `$(PKG_CONFIG) --cflags libusb` -O -Wall
//LIBS            = `$(PKG_CONFIG) --libs libusb`

TMPDIR = /tmp


all: stm32kbdIRconfig

.c.o:
	$(CC) $(CFLAGS) -c $<

stm32kbdIRconfig: stm32kbdIRconfig.o
	$(CC) -o stm32kbdIRconfig stm32kbdIRconfig.o

#$(LIBS)

clean:
	rm -f stm32kbdIRconfig.o
	rm -f stm32kbdIRconfig

dist: clean
	@-rm -rf $(TMPDIR)/$(PROJECTNAME)-$(VERSION)
	@mkdir $(TMPDIR)/$(PROJECTNAME)-$(VERSION)
	@cp -a * $(TMPDIR)/$(PROJECTNAME)-$(VERSION)
	@tar cjf $(PROJECTNAME)-$(VERSION).tar.bz2 -C $(TMPDIR) $(PROJECTNAME)-$(VERSION)
	@-rm -rf $(TMPDIR)/$(PROJECTNAME)-$(VERSION)
	@echo Distribution package created as $(PROJECTNAME)-$(VERSION).tar.bz2
