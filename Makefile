CC = gcc
CPP = gcc -E
CXX = g++
CXXCPP = g++ -E

# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -Wextra
CXXFLAGS += -O9 -mpopcnt

%.o: %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: all clean

all: bitmap_bench

clean:
	rm -f bitmap_bench *.o

bitmap_bench: bitmap.o bitmap_bench.o
	$(CXX) $(CXXFLAGS) $(CXXFLAGS) $^ -o $@
