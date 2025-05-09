.PHONY: all watch run test debug clean

HDRS = $(shell find src/ | grep "\.h" | grep -v macros )
SRCS = $(HDRS:.h=.c)
SRCS += src/main.c

OUT = main.out

TESTS := src/history.c src/str.c
ifndef GITHUB
TESTS += src/path.c
endif

TESTOUT = $(TESTS:.c=.out)

CFLAGS = -p -g -Wall -Wextra -Werror -Wpedantic -Wshadow -Wconversion -Wformat=2 \
         -Wcast-align -Wstrict-overflow=5 -Wundef -Wswitch-default -Wswitch-enum \
         -Wfloat-equal -Wcast-qual -Wbad-function-cast -Wwrite-strings -Wmissing-declarations \
         -Wmissing-prototypes -Wnested-externs -Wlogical-op -Wduplicated-cond -Wduplicated-branches \
         -Wnull-dereference -Wjump-misses-init -Wdouble-promotion -Wvla -Wstack-protector

all: run

debug:
	clear
	gcc $(CFLAGS) $(SRCS) -o $(OUT) 

run: debug
	./$(OUT)

watch:
	watch -n 0.1 tail -n 10 b.txt

valgrind: debug
	DEBUGINFOD_URLS="https://debuginfod.archlinux.org/" valgrind ./$(OUT)

gdb: debug
	gdb ./$(OUT)

test: $(TESTS)
	@$(foreach f, $(TESTS), gcc $(CFLAGS) $(f) -o $(f:.c=.out) -DTEST ; echo Testing $(f)... ; ./$(f:.c=.out);)

valgrind-test: $(TESTS)
	@$(foreach f, $(TESTS), DEBUGINFOD_URLS="https://debuginfod.archlinux.org/" valgrind ./$(f:.c=.out); )
	
clean:
	rm -f *.out *.txt src/*.out
