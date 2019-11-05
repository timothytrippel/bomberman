# -----------------------------------------------------------------------------
# File:        Makefile
# Author:      Timothy Trippel
# Affiliation: MIT Lincoln Laboratory
# Description:

# This Makefile builds a custom backend target module for the Icarus 
# Verilog compiler frontend.
# -----------------------------------------------------------------------------

INSTALL      := /usr/bin/install -c
INSTALL_DATA := ${INSTALL} -m 644

# Directory Structure
IVL_DIR     := ../iverilog
# IVL_DIR     := /Users/ti27457/Repos/iverilog
INCLUDE_DIR := include
SRC_DIR     := src
BUILD_DIR   := build
BIN_DIR     := $(IVL_DIR)/lib/ivl

# Compiler Configurations
CC          := g++
CFLAGS      := -I$(IVL_DIR) -I$(INCLUDE_DIR) -fpic -std=gnu++0x -Wall -Wextra -Wshadow -g -O2
# CFLAGS      := -I$(IVL_DIR) -I$(INCLUDE_DIR) -fpic -shared -std=gnu++0x -Wall -Wextra -Wshadow -g -O2

# Source/Target Files
TGT = ttb
SRCS := $(shell find $(SRC_DIR) -type f -name *.cc)
OBJS := $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(SRCS:.cc=.o))

# Make Targets
all: 
	$(MAKE) $(TGT) 
	$(MAKE) confs 
	$(MAKE) install

$(TGT): $(OBJS)
	$(CC) -undefined suppress -flat_namespace $(CFLAGS) -o $@.tgt $^ 
# 	$(CC) -flat_namespace $(CFLAGS) -o $@.tgt $^ 
# 	$(CC) $(CFLAGS) -o $@.tgt $^ 

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cc
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

confs: ./generate_tgt_confs.sh
	./generate_tgt_confs.sh $(TGT)

.PHONY: install uninstall clean cleanall

install:
	$(INSTALL)      ./$(TGT).tgt  "$(BIN_DIR)/$(TGT).tgt"
	$(INSTALL_DATA) $(TGT).conf   "$(BIN_DIR)/$(TGT).conf"
	$(INSTALL_DATA) $(TGT)-s.conf "$(BIN_DIR)/$(TGT)-s.conf"

uninstall:
	rm -f $(BIN_DIR)/$(TGT).tgt
	rm -f $(BIN_DIR)/$(TGT).conf
	rm -f $(BIN_DIR)/$(TGT)-s.conf

clean:
	rm -rf $(BUILD_DIR)
	rm -f *.tgt
	rm -f *.conf

cleanall: clean uninstall
