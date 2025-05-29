.PHONY: clean rebuild

rebuild: clean build
	@cd build && cmake .. && make -j2 && cd .. && cp ./build/server ./server && mkdir logs

clean:
	rm -f server 
	rm -rf build 

build:
	mkdir -p build 
