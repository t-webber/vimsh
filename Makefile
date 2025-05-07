.PHONY: all watch run test debug clean

HDRS = $(shell find src/ | grep "\.h" | grep -v macros )
SRCS = $(HDRS:.h=.c)
SRCS += src/main.c

OUT = main.out

TESTS := src/history.h src/str.h 


CFLAGS = -Wall -Wextra -Werror -Wpedantic -Wshadow -Wconversion -Wformat=2 \
         -Wcast-align -Wstrict-overflow=5 -Wundef -Wswitch-default -Wswitch-enum \
         -Wfloat-equal -Wcast-qual -Wbad-function-cast -Wwrite-strings -Wmissing-declarations \
         -Wmissing-prototypes -Wnested-externs -Wlogical-op -Wduplicated-cond -Wduplicated-branches \
         -Wnull-dereference -Wjump-misses-init -Wdouble-promotion -Wvla -Wstack-protector

all: run 

debug:
	clear
	gcc -g $(CFLAGS) $(SRCS) -o $(OUT) 

run: debug
	./$(OUT)

watch:
	watch -n 0.1 tail -n 10 b.txt

valgrind: debug
	DEBUGINFOD_URLS="https://debuginfod.archlinux.org/" valgrind ./$(OUT)
	
gdb: debug
	gdb ./$(OUT)

test: 
	@$(foreach f, $(TESTS), gcc $(CFLAGS) $(f:.h=.c) -o $(f:.h=.out) -DTEST ; ./$(f:.h=.out);)

clean:
	rm -f *.out *.txt
