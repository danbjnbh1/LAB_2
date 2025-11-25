all: build myshell build/Looper mypipe

# Create build directory
build:
	mkdir -p build

# Link myshell executable to root/
myshell: build/myshell.o build/LineParser.o
	gcc -g -m32 -Wall -o myshell build/myshell.o build/LineParser.o

# Link Looper executable to build/
build/Looper: build/Looper.o
	gcc -g -m32 -Wall -o build/Looper build/Looper.o

# Link mypipe executable to root/
mypipe: build/mypipe.o
	gcc -g -m32 -Wall -o mypipe build/mypipe.o

# Compile C files to build directory
build/myshell.o: myshell.c LineParser.h
	gcc -g -m32 -Wall -c myshell.c -o build/myshell.o

build/Looper.o: Looper.c
	gcc -g -m32 -Wall -c Looper.c -o build/Looper.o

build/mypipe.o: mypipe.c
	gcc -g -m32 -Wall -c mypipe.c -o build/mypipe.o

build/LineParser.o: LineParser.c LineParser.h
	gcc -g -m32 -Wall -c LineParser.c -o build/LineParser.o

clean:
	rm -rf build myshell mypipe

.PHONY: all clean