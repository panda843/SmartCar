target : Api.o Device.o Sock.o     
	g++ -o main Api.o Device.o Sock.o  main.cpp -lpthread
Api.o : Api.cpp
	g++ -c Api.cpp
Device.o : Device.cpp
	g++ -c Device.cpp
Sock.o : Sock.cpp
	g++ -c Sock.cpp
clean:
	rm -rf *.o main