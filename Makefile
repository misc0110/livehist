all: livehist

livehist: livehist.o getopt_helper.o
	g++ livehist.o getopt_helper.o -o livehist

livehist.o: livehist.cpp
	g++ livehist.cpp -O3 -Wall -c
	
getopt_helper.o: getopt_helper.cpp
	g++ getopt_helper.cpp -O3 -Wall -c

clean:
	rm -f livehist *.o
	
