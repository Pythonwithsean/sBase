name := "Sean"
sources := $(wildcard ./core/src/*.cpp ./core/utils/*.cpp)
objects := foo.o soo.o

.PHONY: build run clean
.SILENT:

build: 
	g++ -std=c++23 $(sources) -g -O1 -I./include -o ./build/Sbase

run: 
	./build/Sbase
clean: 
	rm -rf ./build/*
	rm -rf ./config/*
	echo "Cleaned"

sean: ./core/src/DB.cpp
	echo $(name) $^
