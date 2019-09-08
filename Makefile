all: livehist

livehist: livehist.o cli_parser.o
	g++ livehist.o cli_parser.o -o livehist

livehist.o: livehist.cpp
	g++ livehist.cpp -O3 -Wall -c
	
cli_parser.o: cli_parser.cpp
	g++ cli_parser.cpp -O3 -Wall -c

clean:
	rm -f livehist *.o
	
install:
	cp livehist /usr/bin/

uninstall:
	rm /usr/bin/livehist
