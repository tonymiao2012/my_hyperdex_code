
MPI_HOME     = /usr/local/
CC           = mpicc
CFLAGS       = -g
CPPFLAGS     = -I$(MPI_HOME)/include
LDFLAGS      = -L$(MPI_HOME)/lib -lhyperclient

EXECS        = kvs_put kvs_get

all: $(EXECS)


kvs_get: kvs_get.o
	$(CC) -o $@ $^ $(LDFLAGS)

kvs_put: kvs_put.o
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o kvs_put kvs_get

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<
