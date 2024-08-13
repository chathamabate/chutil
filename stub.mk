

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
LIB_DIR:=$(PROJ_DIR)/$(LIB_NAME)

include $(PROJ_DIR)/vars.mk

# Each library will be built entirely independently to its
# dependencies. It will search for libraries in the install path.

HEADERS		:=$(wildcard include/*.h)
OBJS		:=$(patsubst %.c,%.o,$(SRCS))
FULL_OBJS	:=$(addprefix build/,$(OBJS))

TEST_HEADERS	:=$(wildcard test/*.h)
TEST_OBJS		:=$(patsubst %.c,%.o,$(TEST_SRCS))
FULL_TEST_OBJS	:=$(addprefix build/test/,$(TEST_OBJS))

INCLUDE_PATHS	:=$(LIB_DIR)/include
INCLUDE_PATHS	+=$(INSTALL_DIR)/include
INCLUDE_FLAGS	:=$(addprefix -I,$(INCLUDE_PATHS))

# Where to look for static library dependencies.
DEPS_PATHS	:=$(INSTALL_DIR)
DEPS_FLAGS	:=$(addprefix -L,$(DEPS_PATHS))
DEPS_FLAGS	+=$(foreach dep,$(DEPS),-l$(dep))

build:
	mkdir build

build/%.o:
