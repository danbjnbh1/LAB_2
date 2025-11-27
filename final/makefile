all: myshell mypipe

# Link myshell executable to root/
myshell: myshell.o LineParser.o
	gcc -g -m32 -Wall -o myshell myshell.o LineParser.o

# Link mypipe executable to root/
mypipe: mypipe.o
	gcc -g -m32 -Wall -o mypipe mypipe.o

# Compile C files to root directory
myshell.o: myshell.c LineParser.h
	gcc -g -m32 -Wall -c myshell.c -o myshell.o

mypipe.o: mypipe.c
	gcc -g -m32 -Wall -c mypipe.c -o mypipe.o

LineParser.o: LineParser.c LineParser.h
	gcc -g -m32 -Wall -c LineParser.c -o LineParser.o

clean:
	rm -rf myshell mypipe LineParser.o myshell.o mypipe.o

.PHONY: all clean