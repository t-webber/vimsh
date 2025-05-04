.PHONY: all debug run clean

all: run

debug:
				gcc -g src/main.c -o main.out 

run: debug 
				./main.out

clean:
				rm -f main.out
