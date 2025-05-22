name := "Sean"
sources := $(wildcard ./core/src/*.cpp ./core/utils/*.cpp)
test_sources := $(wildcard ./tests/*.cpp)
gtest_sources := ./external/googletest/googletest/src/gtest_main.cc ./external/googletest/googletest/src/gtest-all.cc
app_sources := ./core/src/Repl.cpp
common_sources := $(filter-out $(app_sources), $(wildcard ./core/src/*.cpp)) $(wildcard ./core/utils/*.cpp)


.PHONY: build run test clean
.SILENT:


build:
	echo "Compiling..."
	g++ -std=c++23 $(sources) -g -O1 \
	-I./include \
	-o ./build/Sbase -pthread

test:
	echo "Compiling tests..."
	g++ -std=c++23 $(common_sources) $(test_sources) $(gtest_sources) -g -O1 \
	-I./include \
	-I./external/googletest/googletest/include \
	-I./external/googletest/googletest \
	-I./tests \
	-pthread -o ./build/Sbase_test

run:
	./build/Sbase

run_test:
	./build/Sbase_test

clean:
	rm -rf ./build/*
	rm -rf ./config/*
	echo "Cleaned"

sean: ./core/src/DB.cpp
	echo $(name) $^
