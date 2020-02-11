CC=gcc
CFLAGS=-lsqlite3 -lipify
ipmax_1.0: ipmax_1.0.o ipmax_functions.o
	$(CC) ipmax_1.0.o ipmax_functions.o -o ipmax_1.0 $(CFLAGS)
ipmax_1.0.o: ipmax_functions.o
	$(CC) -c ipmax_1.0.c -o ipmax_1.0.o
ipmax_functions.o: ipmax_functions.c ipmax_functions.h
	$(CC) -c ipmax_functions.c -o ipmax_functions.o $(CFLAGS)
#%.o : %.c
#	$(CC) -c $< -o $@ $(CFLAGS)
#ipmax_functions.o: ipmax_functions.h
#ipmax_1.0: ipmax_1.0.o ipmax_functions.o
#	$(CC) ipmax_1.0.o ipmax_functions.o -o ipmax_1.0 $(CFLAGS)
install:
	cp ipmax_1.0 /usr/bin
clean:
	rm -v *.o
