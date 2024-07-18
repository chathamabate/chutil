
CC:=gcc
FLAGS:=-Wall -Wextra -Wpedantic -std=c11

CUR_DIR:=$(shell pwd)
ABS_BUILD_PATH:=$(CUR_DIR)/build

LIB_NAME:=chutil
LIB_FILE:=build/lib$(LIB_NAME).a

REL_INC_PATHS:=include
ABS_INC_PATHS:=$(addprefix $(CUR_DIR)/,$(REL_INC_PATHS))

INC_FLAGS:=$(addprefix -I,$(ABS_INC_PATHS))

HEADERS:=$(wildcard include/chutil/*.h)

SRCS:=dummy.c debug.c

OBJS:=$(patsubst %.c,%.o,$(SRCS))
FULL_OBJS:=$(addprefix build/,$(OBJS))

TEST_SRCS:=main.c dummy.c
TEST_HEADER:=$(wildcard test/*.h)
TEST_OBJS:=$(patsubst %.c,%.o,$(TEST_SRCS))
FULL_TEST_OBJS:=$(addprefix build/test/,$(TEST_OBJS))

.PHONY: all clean clangd

all: $(LIB_FILE) build/test/test

build:
	mkdir build

build/%.o: src/%.c $(HEADERS) | build
	$(CC) -c $(FLAGS) $(INC_FLAGS) $< -o $@

$(LIB_FILE): $(FULL_OBJS)
	ar rcs $@ $^ 

build/test:
	mkdir build/test

build/test/%.o: test/%.c $(HEADERS) $(TEST_HEADERS) | build/test
	$(CC) -c $(FLAGS) $(INC_FLAGS) $< -o $@

build/test/test: $(FULL_TEST_OBJS) $(LIB_FILE)
	$(CC) $^ -L$(ABS_BUILD_PATH) -lchutil -o $@

clangd:
	echo "CompileFlags:" > .clangd
	echo "  Add:" >> .clangd
	$(foreach flag,$(INC_FLAGS),echo "    - $(flag)" >> .clangd;) 

clean:
	rm -rf build

