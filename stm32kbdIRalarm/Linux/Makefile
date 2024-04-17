
PROJECTNAME=stm32kbdIRalarm

CC              = g++

CFLAGS          = -O -Wall

TMPDIR = /tmp


all: stm32kbdIRalarm

.c.o:
	$(CC) $(CFLAGS) -c $<

stm32kbdIRalarm: stm32kbdIRalarm.o
	$(CC) -o stm32kbdIRalarm stm32kbdIRalarm.o

clean:
	rm -f stm32kbdIRalarm.o
	rm -f stm32kbdIRalarm
