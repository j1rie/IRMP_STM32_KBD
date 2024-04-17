
PROJECTNAME=stm32kbdstatusled

CC              = g++

CFLAGS          = -O -Wall

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
