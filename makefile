all:

	g++ -std=c++11 AWS.cpp -o AWS
	g++ -std=c++11 ServerA.cpp -o ServerA
	g++ -std=c++11 ServerB.cpp -o ServerB
	g++ -std=c++11 Client.cpp -o Client

.PHONY: ServerA ServerB AWS

ServerA:

	./ServerA


ServerB:

	./ServerB


AWS:
	./AWS
