
PROJECTNAME=stm32kbd2uinput
VERSION=0.0.1

CC              = g++

CFLAGS          = -O -Wall

all: stm32kbd2uinput

.c.o:
	$(CC) $(CFLAGS) -c $<

stm32kbd2uinput: stm32kbd2uinput.o
	$(CC) -o stm32kbd2uinput stm32kbd2uinput.o

clean:
	rm -f stm32kbd2uinput.o
	rm -f stm32kbd2uinput

dist: clean
	@-rm -rf $(TMPDIR)/$(PROJECTNAME)-$(VERSION)
	@mkdir $(TMPDIR)/$(PROJECTNAME)-$(VERSION)
	@cp -a * $(TMPDIR)/$(PROJECTNAME)-$(VERSION)
	@tar cjf $(PROJECTNAME)-$(VERSION).tar.bz2 -C $(TMPDIR) $(PROJECTNAME)-$(VERSION)
	@-rm -rf $(TMPDIR)/$(PROJECTNAME)-$(VERSION)
	@echo Distribution package created as $(PROJECTNAME)-$(VERSION).tar.bz2
