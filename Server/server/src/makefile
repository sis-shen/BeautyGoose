LDLIBS = -lmysqlcppconn -ljsoncpp -lpthread

server:main.o DatabaseClient.o HTTPServer.o
	g++ -o $@ *.o -std=c++17 $(LDLIBS)

server-static:main.o DatabaseClient.o HTTPServer.o
	g++ 

main.o: main.cpp
	g++ -c main.cpp -o main.o -std=c++17 

DatabaseClient.o:DatabaseClient.cpp 
	g++ -c DatabaseClient.cpp -o DatabaseClient.o -std=c++17 

HTTPServer.o:HTTPServer.cpp
	g++ -c HTTPServer.cpp -o HTTPServer.o -std=c++17 

.PHONY:clean
clean:
	rm -f server *.o