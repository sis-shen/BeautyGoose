.PHONY: clean rebuild

rebuild: clean build
	@cd build && cmake .. && make -j2 && cd .. && cp ./build/server ./server && mkdir logs

clean:
	rm -f server 
	rm -rf build 

build:
	mkdir -p build 



# LDLIBS = -lmysqlcppconn -ljsoncpp -lpthread -lgflags -lfmt -lredis++ -lhiredis

# SRCS = $(wildcard *.cpp)
# OBJS = $(SRCS:.cpp=.o)

# all:server

# server: $(OBJS)
# 	g++ -o $@ *.o -std=c++17 $(LDLIBS)

# server_g: $(OBJS)
# 	g++ -fno-omit-frame-pointer -O0 -g -o server *.o -std=c++17 $(LDLIBS)


# %.o: %.cpp
# 	g++ -c $< -o $@

# .PHONY:clean
# clean:
# 	rm -f server *.o