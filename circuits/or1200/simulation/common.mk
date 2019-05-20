# -----------------------------------------------------------------------------
# File:        Makefile
# Author:      Timothy Trippel
# Affiliation: MIT Lincoln Laboratory
# Description:

# This Makefile to builds a benchmark VMEM program for simulating the OR1200.
# -----------------------------------------------------------------------------

# Directory Structure
UTILS_DIR   := ../../utils
INCLUDE_DIR := ../../include
BTLDR_DIR   := ../../bootloader

# Compiler Configurations
OPTLVL := -O3
CC     := or1k-elf-gcc
CFLAGS := -I$(INCLUDE_DIR) -Wall $(OPTLVL)
LIBS   := -lc -lm
# CFLAGS := -I$(INCLUDE_DIR) -Wall $(OPTLVL) -ffreestanding -std=c99 -fomit-frame-pointer -fno-optimize-sibling-calls

# Make Targets
all: $(BENCHMARK).vmem

# Convert object file to VMEM file
%.vmem: %.bin $(UTILS_DIR)/bin2vmem
	$(UTILS_DIR)/bin2vmem $< > $@

# Convert ELF file to binary file
%.bin: %.elf
	or1k-elf-objdump -D $< > $<.asm; \
	or1k-elf-objcopy -O binary $< $@

# Cross-Compile C program to ELF file
%.elf: $(OBJS)
	$(CC) $(CFLAGS) $(BTLDR_DIR)/bootloader.S $^ -o $@ $(LIBS)

# Cross-Compile C program to object file
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: all binutils clean cleanall

# Build binary formating utils
$(UTILS_DIR)/bin2vmem:
	$(MAKE) -C $(UTILS_DIR)

cleanall: clean
	$(MAKE) clean -C $(UTILS_DIR)	

clean:
	rm -f *.o *.bin *.elf.asm *.vmem

# For debugging
print-%  : ; @echo $* = $($*)