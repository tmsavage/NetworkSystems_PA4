CC=gcc
CFLAGS=-pthread

all: proxy

clean:
		rm proxy

proxy: httpechosrv-basic.c
		$(CC) $(CFLAGS) httpechosrv-basic.c -o proxy
