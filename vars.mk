
# Global Makefile variables used in stub.mk

PROJ_DIR:=$(shell git rev-parse --show-toplevel)

CC:=gcc
FLAGS:=-Wall -Wextra -Wpedantic -std=c11

# Where to search for static libraries and header files
# of other modules.
INSTALL_DIR?=$(PROJ_DIR)/install

