all: build build/Looper

# Create build directory
build:
	mkdir -p build

# Link Looper executable to build/
build/Looper: build/Looper.o
	gcc -g -m32 -Wall -o build/Looper build/Looper.o

# Compile C files to build directory
build/Looper.o: Looper.c
	gcc -g -m32 -Wall -c Looper.c -o build/Looper.o

build/LineParser.o: LineParser.c
	gcc -g -m32 -Wall -c LineParser.c -o build/LineParser.o

clean:
	rm -rf build

.PHONY: all clean