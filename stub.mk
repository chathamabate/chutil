
# This stub file is meant to be included
# at the bottom of every library's make file.
# 
# It expects the follow defines:
# 
# LIB_NAME	:=
# DEPS		:=
# SRCS		:=
# TEST_SRCS :=
#
# It also expects each library to have the following structure:
#
# src: just .c files.
# include: just .h files.
# test: .c and .h files for building test binary.
#
# Output will be placed in build.
# Each library will have its own build folder.
# Use the install target to copy build library and headers

PROJ_DIR:=$(shell git rev-parse --show-toplevel)

# I like absolute paths tbh.
LIB_DIR		:=$(PROJ_DIR)/$(LIB_NAME)

INCLUDE_DIR	:=$(LIB_DIR)/include
SRC_DIR		:=$(LIB_DIR)/src
TEST_DIR	:=$(LIB_DIR)/test

BUILD_DIR		:=$(LIB_DIR)/build
BUILD_TEST_DIR	:=$(BUILD_DIR)/test

LIB_FILE_NAME	:=lib$(LIB_NAME).a
LIB_FILE		:=$(BUILD_DIR)/$(LIB_FILE_NAME)

include $(PROJ_DIR)/vars.mk

# Each library will be built entirely independently to its
# dependencies. It will search for libraries in the install path.

HEADERS		:=$(wildcard $(INCLUDE_DIR)/*.h)
OBJS		:=$(patsubst %.c,%.o,$(SRCS))
FULL_OBJS	:=$(addprefix $(BUILD_DIR)/,$(OBJS))

TEST_HEADERS	:=$(wildcard $(TEST_DIR)/*.h)
TEST_OBJS		:=$(patsubst %.c,%.o,$(TEST_SRCS))
FULL_TEST_OBJS	:=$(addprefix $(BUILD_TEST_DIR)/,$(TEST_OBJS))

# It is important our local include is first.
# This should be searched first.
INCLUDE_PATHS	:=$(INCLUDE_DIR) $(INSTALL_DIR)/include
INCLUDE_FLAGS	:=$(addprefix -I,$(INCLUDE_PATHS))

# Where to look for static library dependencies.
# Again, it is important our local build is first.
DEPS_PATHS 	:=$(BUILD_DIR) $(INSTALL_DIR)
DEPS_FLAGS	:=$(addprefix -L,$(DEPS_PATHS)) $(foreach dep,$(DEPS),-l$(dep))

.PHONY: all lib test uninstall install clean clangd

all: lib test

lib: $(LIB_FILE)

test: $(BUILD_TEST_DIR)/test

uninstall:
	rm -rf $(INSTALL_DIR)/include/$(LIB_NAME)
	rm -f $(INSTALL_DIR)/$(LIB_FILE_NAME)

install: uninstall $(LIB_FILE)
	cp -r $(INCLUDE_DIR)/$(LIB_NAME) $(INSTALL_DIR)/include
	cp $(LIB_FILE) $(INSTALL_DIR)

clean:
	rm -rf $(BUILD_DIR)

clangd:
	echo "CompileFlags:" > .clangd
	echo "  Add:" >> .clangd
	$(foreach flag,$(INCLUDE_FLAGS),echo "    - $(flag)" >> .clangd;) 

$(BUILD_DIR):
	mkdir $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) | $(BUILD_DIR)
	$(CC) -c $(FLAGS) $(INCLUDE_FLAGS) $< -o $@

# Remember, the lib file doesn't actually care about dependencies.
$(LIB_FILE): $(FULL_OBJS)
	ar rcs $@ $^

$(BUILD_TEST_DIR):
	mkdir $@

$(BUILD_TEST_DIR)/%.o: $(TEST_DIR)/%.c $(HEADERS) $(TEST_HEADERS) | $(BUILD_TEST_DIR)
	$(CC) -c $(FLAGS) $(INCLUDE_FLAGS) $< -o $@

# Always include unity!
$(BUILD_TEST_DIR)/test: $(FULL_TEST_OBJS) $(LIB_FILE)
	$(CC) $(DEPS_FLAGS) -lunity $^ -o $@

