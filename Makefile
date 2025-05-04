.PHONY: all debug clean

HDRS  = $(shell find src/ | grep \.h$ )
SRCS  = $(HDRS:.h=.c)
SRCS += src/main.c

all: debug 

debug:
				gcc -g $(SRCS) -o main.out 

clean:
				rm -f main.out *.txt
