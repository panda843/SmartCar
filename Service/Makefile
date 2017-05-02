target : Api.o Device.o     
	g++ -o main Api.o Device.o  main.cpp -lpthread -levent
Api.o : Api.cpp
	g++ -levent -c Api.cpp
Device.o : Device.cpp
	g++ -c Device.cpp
clean:
	rm -rf *.o main