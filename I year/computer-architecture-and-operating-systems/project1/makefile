CC       = gcc
CFLAGS   = -Wall -Wextra -Wno-implicit-fallthrough -std=gnu17 -fPIC -O2
LDFLAGS  = -shared -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc -Wl,--wrap=reallocarray -Wl,--wrap=free -Wl,--wrap=strdup -Wl,--wrap=strndup


all: libseq.so seq_test.out

libseq.so: seq.o memory_tests.o
	$(CC) $(LDFLAGS) -o libseq.so seq.o memory_tests.o

seq.o: seq.c seq.h
	$(CC) $(CFLAGS) -c seq.c

memory_tests.o: memory_tests.c memory_tests.h
	$(CC) -std=gnu17 -fPIC -c memory_tests.c

seq_test.out: seq_example.c libseq.so
	$(CC) seq_example.c -L. $(CFLAGS) -o seq_test.out -lseq

clean:
	rm -f *.o *.so *.out
