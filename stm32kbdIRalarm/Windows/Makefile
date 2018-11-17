all: stm32kbdIRalarm

CC=gcc
CXX=g++
COBJS=hid.o
CPPOBJS=stm32kbdIRalarm.o
OBJS=$(COBJS) $(CPPOBJS)
CFLAGS=-g -c -Wall
ifeq ($(MSYSTEM),MINGW32)
LIBS= -lsetupapi -static-libgcc
else ifeq ($(MSYSTEM),MINGW64)
LIBS= -lsetupapi
endif
CXXFLAGS=-g -s

stm32kbdIRalarm: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o stm32kbdIRalarm

$(COBJS): %.o: %.c
	$(CC) $(CFLAGS) $< -o $@

$(CPPOBJS): %.o: %.cpp
	$(CXX) $(CFLAGS) $< -o $@

clean:
	rm -f *.o stm32kbdIRalarm.exe

.PHONY: clean
