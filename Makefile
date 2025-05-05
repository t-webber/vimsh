.PHONY: all watch run test debug clean

HDRS  = $(shell find src/ | grep "\.h" )
SRCS  = $(HDRS:.h=.c)
SRCS += src/main.c

OUT = main.out

TESTS := src/history.h src/str.h 

all: run 

debug:
	gcc -g $(SRCS) -o $(OUT) 

run: debug
	./$(OUT)

watch:
	watch -n 0.1 tail -n 10 b.txt

valgrind: debug
	DEBUGINFOD_URLS="https://debuginfod.archlinux.org/" valgrind ./$(OUT)
	
	

test: 
	@$(foreach f, $(TESTS), gcc $(f) $(f:.h=.c) -o $(f:.h=.out) -DTEST ; ./$(f:.h=.out);)

clean:
	rm -f *.out *.txt
