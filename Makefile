.PHONY: all watch run test debug clean

HDRS = $(shell find src/ | grep "\.h" | grep -v macros )
SRCS = $(HDRS:.h=.c)

OUT = main.out

TESTS := src/history.c src/str.c
ifndef GITHUB
TESTS += src/path.c
endif

TESTOUT = $(TESTS:.c=.out)

CFLAGS = -p -g \
	 -Wall -Wextra -Werror -Wpedantic -Wshadow -Wconversion -Wformat=2 \
         -Wcast-align -Wstrict-overflow=5 -Wundef -Wswitch-default -Wswitch-enum \
         -Wfloat-equal -Wcast-qual -Wbad-function-cast -Wwrite-strings -Wmissing-declarations \
         -Wmissing-prototypes -Wnested-externs -Wlogical-op -Wduplicated-cond -Wduplicated-branches \
         -Wnull-dereference -Wjump-misses-init -Wdouble-promotion -Wvla -Wstack-protector


VALGRIND = DEBUGINFOD_URLS="https://debuginfod.archlinux.org/" valgrind 
VALGRIND_FLAGS = --track-origins=yes --show-error-list=yes --show-leak-kinds=all --leak-check=full 

all: valgrind 

debug:
	clear
	gcc $(CFLAGS) $(SRCS) -o $(OUT) 

run: debug
	./$(OUT)

watch:
	watch -n 0.1 tail -n 30 b.txt

valgrind: debug
	$(VALGRIND) $(VALGRIND_FLAGS) ./$(OUT)


gdb: debug
	gdb ./$(OUT)

test: $(TESTS)
	$(foreach f, $(TESTS), gcc $(CFLAGS) $(f) -o $(f:.c=.out) -DTEST ; echo Testing $(f)... ; ./$(f:.c=.out);)

valgrindtest: test
	$(foreach f, $(TESTS), echo -e "===============================\n===============================\n>>>>> Running valgrind for $(f)..." ; $(VALGRIND) $(VALGRIND_FLAGS) ./$(f:.c=.out); )
	
	
clean:
	rm -f *.out *.txt src/*.out vgcore.* callgrind.out.*
