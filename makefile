compile: 
	@g++ -std=c++23 ./core/src/*.cpp -Wall -Wextra -g -O1 -I./include -o ./build/Sbase
run: 
	@./build/Sbase
clean: 
	@rm -rf ./build/*
	@rm -rf ./config/*
	@echo "Cleaned"