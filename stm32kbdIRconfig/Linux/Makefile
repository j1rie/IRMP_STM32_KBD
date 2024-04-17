
PROJECTNAME=stm32kbdIRconfig

CC              = g++

CFLAGS          = -O -Wall
//CFLAGS          = `$(PKG_CONFIG) --cflags libusb` -O -Wall
//LIBS            = `$(PKG_CONFIG) --libs libusb`

all: stm32kbdIRconfig

.c.o:
	$(CC) $(CFLAGS) -c $<

stm32kbdIRconfig: stm32kbdIRconfig.o
	$(CC) -o stm32kbdIRconfig stm32kbdIRconfig.o

#$(LIBS)

clean:
	rm -f stm32kbdIRconfig.o
	rm -f stm32kbdIRconfig
