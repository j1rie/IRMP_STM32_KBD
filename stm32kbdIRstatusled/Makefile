
PROJECTNAME=stm32kbdIRstatusled

CC              = g++

CFLAGS          = -O -Wall

all: stm32kbdIRstatusled

.c.o:
	$(CC) $(CFLAGS) -c $<

stm32kbdIRstatusled: stm32kbdIRstatusled.o
	$(CC) -o stm32kbdIRstatusled stm32kbdIRstatusled.o

clean:
	rm -f stm32kbdIRstatusled.o
	rm -f stm32kbdIRstatusled
