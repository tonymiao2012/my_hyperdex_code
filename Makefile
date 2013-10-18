
TONY_HOME     = /usr/local
CC           = gcc
CFLAGS       = -g
CPPFLAGS     = -I$(TONY_HOME)/include
LDFLAGS      = -L$(TONY_HOME)/lib -lhyperclient

EXECS        = replay

all: $(EXECS)


replay: replay.o
	$(CC)  -o $@ $^  $(LDFLAGS)

clean:
	rm -f *.o replay test.log

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<
