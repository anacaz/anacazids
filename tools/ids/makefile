
CC=	gcc
CFILES=	
OFILES=	$(CFILES:%.c=%.o)
CFLAGS=	-I. -I../../include -I../../include/ids

TARGETS	= mklist mktest showlist bin2ascii showhex

all: clean $(TARGETS)

bin2ascii: bin2ascii.o
	$(CC) -o $@ bin2ascii.o

mklist: mklist.o
	$(CC) -o $@ mklist.o

showlist: showlist.o
	$(CC) -o $@ showlist.o

mktest: mktest.o
	$(CC) -o $@ mktest.o

showhex: showhex.o
	$(CC) -o $@ showhex.o

.c.o: 
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o $(TARGETS)

