# -----------------------------------------------------------------------------
# File:        Makefile
# Author:      Timothy Trippel
# Affiliation: MIT Lincoln Laboratory
# Description:

# This Makefile to builds all VMEM program files for simulating the OR1200.
# -----------------------------------------------------------------------------

# Directory Structure
UTILS_DIR   := utils
INCLUDE_DIR := include
SRC_DIR     := src
BTLDR_DIR   := bootloader
BUILD_DIR   := build
BIN_DIR     := bin
VMEM_DIR    := vmem

# Compiler Configurations
CC     := or1k-elf-gcc
CFLAGS := -I$(INCLUDE_DIR) -Wall

# Source/Target Files
SRCS  := $(shell find $(SRC_DIR) -type f -name *.c)
VMEMS := $(patsubst $(SRC_DIR)/%,$(VMEM_DIR)/%,$(SRCS:.c=.vmem))

# Make Targets
all: $(VMEMS)

# Convert object file to VMEM file
$(VMEM_DIR)/%.vmem: $(BIN_DIR)/%.bin $(UTILS_DIR)/bin2vmem
	@mkdir -p $(VMEM_DIR) && \
	$(UTILS_DIR)/bin2vmem $< > $@

# Convert ELF object file to binary file
$(BIN_DIR)/%.bin: $(BUILD_DIR)/%.elf
	@mkdir -p $(BIN_DIR) && \
	or1k-elf-objdump -D $< > $<.asm && \
	or1k-elf-objcopy -O binary $< $@

# Cross-Compile C program to ELF object file
$(BUILD_DIR)/%.elf: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR) && \
	$(CC) $(CFLAGS) $(BTLDR_DIR)/bootloader.S $< -o $@

.PHONY: all binutils clean

# Build binary formating utils
binutils:
	$(MAKE) -C $(UTILS_DIR)

clean:
	@rm -rf $(BUILD_DIR)
	@rm -rf $(BIN_DIR)
	@rm -rf $(VMEM_DIR)
