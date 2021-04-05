projekt1: makefile main.o receive.o send.o
	g++ -std=c++17 -Wall -Wextra -o projekt1 main.o receive.o send.o

main.o: makefile main.cpp
	g++ -std=c++17 -Wall -Wextra -c main.cpp -o main.o

receive.o: makefile receive.cpp
	g++ -std=c++17 -Wall -Wextra -c receive.cpp -o receive.o

send.o: makefile send.cpp
	g++ -std=c++17 -Wall -Wextra -c send.cpp -o send.o

clean :
	rm -f *.o

distclean :
	rm -f *.o projekt1