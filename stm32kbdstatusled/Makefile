
PROJECTNAME=stm32kbdstatusled

CC              = g++

CFLAGS          = -O -Wall

all: stm32kbdstatusled

.c.o:
	$(CC) $(CFLAGS) -c $<

stm32kbdstatusled: stm32kbdstatusled.o
	$(CC) -o stm32kbdstatusled stm32kbdstatusled.o

clean:
	rm -f stm32kbdstatusled.o
	rm -f stm32kbdstatusled
