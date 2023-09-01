a.out:	matrix.o node.o system.o graph.o main.o
	g++ matrix.o node.o system.o graph.o main.o -lX11

matrix.o: matrix.cpp node.h system.h matrix.h graph.h
	g++ -c matrix.cpp

node.o: node.cpp node.h system.h matrix.h graph.h
	g++ -c node.cpp

system.o: system.cpp node.h system.h matrix.h graph.h
	g++ -c system.cpp

graph.o: graph.cpp node.h system.h matrix.h graph.h
	g++ -c graph.cpp

main.o: main.cpp node.h system.h matrix.h graph.h
	g++ -c main.cpp

clean:
	rm *.o
