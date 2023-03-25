CC = g++ 
CFLAGS  =  -std=c++0x -Wall

GROFF = groff -Tascii -T pdf -man

default:	server client client.pdf server.pdf

client.pdf: client.man
	cat client.man | $(GROFF) > client.pdf

server.pdf: server.man
	cat server.man | $(GROFF) > server.pdf

server: server.o tands.o loglog.o timetime.o
	$(CC) $(CFLAGS) -o server server.o tands.o loglog.o timetime.o

client: client.o tands.o loglog.o timetime.o
	$(CC) $(CFLAGS) -o client client.o tands.o loglog.o timetime.o

server.o: server.cpp tands.h loglog.h timetime.h
	$(CC) $(CFLAGS) -c server.cpp

client.o: client.cpp tands.h loglog.h timetime.h
	$(CC) $(CFLAGS) -c client.cpp

loglog.o: loglog.cpp loglog.h
	$(CC) $(CFLAGS) -c loglog.cpp

tands.o: tands.cpp tands.h
	$(CC) $(CFLAGS) -c tands.cpp

timetime.o: timetime.cpp timetime.h
	$(CC) $(CFLAGS) -c timetime.cpp

clean:
	$(RM) count *.o *~
