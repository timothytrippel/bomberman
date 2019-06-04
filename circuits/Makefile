# Directory Structure
TGT_TTB_DIR :=../tgt-ttb
SCRIPTS     :=../scripts
AES         := aes
UART        := uart
OR1200      := or1200
OR1200_SIM  := $(OR1200)/simulation
OR1200_VMEM := $(OR1200_SIM)/vmem

# OR1200 Simulation Program
# OR1200_PROGRAM := $(OR1200_VMEM)/hello_world.vmem
OR1200_PROGRAM := $(OR1200_VMEM)/basicmath.vmem

all: tgt-ttb aes uart or1200

# Circuit Analyses
aes:
	@$(MAKE) -C $(AES)

uart:
	@$(MAKE) -C $(UART)

or1200:
	@$(MAKE) -C $(OR1200)

# or1200: $(OR1200_PROGRAM)
# 	@echo "Starting $@ analysis..."; \
# 	cp $(OR1200_PROGRAM) $(OR1200)/$(TYPE)/sram.vmem; \
# 	if [ $(LOG) == 0 ]; then \
# 		$(MAKE) -C $(OR1200) \
# 	else \
# 		$(MAKE) -C $(OR1200) &> $(OUT_DIR)/$@.log; \
# 	fi; \
# 	echo "Finished $@ analysis."

# %.vmem:
# 	$(MAKE) -C $(OR1200_SIM)

# Building the TTB IVL Target Module
tgt-ttb: 
	$(MAKE) -C $(TGT_TTB_DIR)

.PHONY: clean clean-aes clean-uart clean-or1200 tgt-ttb aes uart or1200

clean: clean-aes clean-uart clean-or1200

clean-aes:
	$(MAKE) clean -C $(AES)

clean-uart:
	$(MAKE) clean -C $(UART)

clean-or1200:
	$(MAKE) clean -C $(OR1200)
