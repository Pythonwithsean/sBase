name := "Sean"

.PHONY: build

build: 
	@g++ -std=c++23 ./core/src/*.cpp ./core/utils/*.cpp -g -O1 -I./include -o ./build/Sbase

run: 
	@./build/Sbase
clean: 
	@rm -rf ./build/*
	@rm -rf ./config/*
	@echo "Cleaned"

test: ./core/src/DB.cpp
	@echo Hello $(name) $?
