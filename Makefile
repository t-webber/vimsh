.PHONY: all watch run test debug clean

HDRS  = $(shell find src/ | grep "\.h" )
SRCS  = $(HDRS:.h=.c)
SRCS += src/main.c

OUT = main.out

all: run 

debug:
	gcc -g $(SRCS) -o $(OUT) 

run: debug
	./$(OUT)

watch:
	watch -n 0.1 tail -n 10 b.txt

test: 
	gcc src/str.c -DTEST -o str_test.out
	./str_test.out

clean:
	rm -f *.out *.txt
