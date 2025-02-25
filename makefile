b: 
	@g++ -std=c++23 ./core/src/*.cpp -Wall -I./include -o ./build/Sbase
r: 
	@./build/Sbase
c: 
	@rm -rf ./build/*
	@echo "Cleaned"