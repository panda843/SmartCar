target : Api.o Device.o Mysql.o
	g++ -g -o main Api.o Device.o Mysql.o  main.cpp -lpthread -levent -lmysqlclient
Mysql.o : Mysql.cpp
	g++ -c Mysql.cpp
Api.o : Api.cpp
	g++ -levent -c Api.cpp
Device.o : Device.cpp
	g++ -c Device.cpp
clean:
	rm -rf *.o main