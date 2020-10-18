VERSION := $(shell cat VERSION)
CFLAGS += -std=c++11
all: livehist

livehist: livehist.o cli_parser.o
	g++ $(CFLAGS) livehist.o cli_parser.o -o livehist

livehist.o: livehist.cpp VERSION
	g++ $(CFLAGS) livehist.cpp -O3 -Wall -DAPPVERSION='"$(VERSION)"' -c
	
cli_parser.o: cli_parser.cpp VERSION
	g++ $(CFLAGS) cli_parser.cpp -O3 -Wall -DAPPVERSION='"$(VERSION)"' -c

clean:
	rm -f livehist *.o
	
install: livehist
	cp livehist /usr/bin/

uninstall:
	rm /usr/bin/livehist
