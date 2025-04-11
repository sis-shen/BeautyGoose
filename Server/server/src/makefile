LDLIBS = -lmysqlcppconn -ljsoncpp -lpthread -lgflags -lfmt -lredis++ -lhiredis

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

all:server

server: $(OBJS)
	g++ -o $@ *.o -std=c++17 $(LDLIBS)

server_g: $(OBJS)
	g++ -fno-omit-frame-pointer -O0 -g -o server *.o -std=c++17 $(LDLIBS)


%.o: %.cpp
	g++ -c $< -o $@

.PHONY:clean
clean:
	rm -f server *.o