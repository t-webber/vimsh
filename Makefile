.PHONY: all debug clean

HDRS  = $(shell find src/ | grep \.h$ )
SRCS  = $(HDRS:.h=.c)
SRCS += src/main.c

all: debug 

debug:
				gcc -g $(SRCS) -o main.out 

watch:
		watch -n 0.1 tail -n 10 b.txt

clean:
				rm -f main.out *.txt
