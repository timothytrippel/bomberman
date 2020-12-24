# Copyright © 2019, Massachusetts Institute of Technology
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

INSTALL      := /usr/bin/install -c
INSTALL_DATA := ${INSTALL} -m 644

# Directory Structure
IVL_DIR   := ../iverilog
SRC_DIR   := src
BUILD_DIR := build
BIN_DIR   := $(IVL_DIR)/lib/ivl

# Compiler Configurations
CC := g++
CFLAGS := \
	-I../ \
	-fpic \
	-shared \
	-std=gnu++0x \
	-Wall \
	-Wextra \
	-Wshadow \
	-g \
	-O3

# Sources/Objects
TGT = dfg
SRCS := $(shell find $(SRC_DIR) -type f -name *.cc)
OBJS := $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(SRCS:.cc=.o))

all: 
	$(MAKE) $(TGT) 
	$(MAKE) confs 
	$(MAKE) install

$(TGT): $(OBJS)
	$(CC) $(CFLAGS) -o $@.tgt $^ 

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
