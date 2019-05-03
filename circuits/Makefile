# Directory Structure
TGT_TTB_DIR :=../tgt-ttb
SCRIPTS     :=../scripts
AES         := aes
UART        := uart
OR1200      := or1200
OR1200_SIM  := $(OR1200)/simulation
OR1200_VMEM := $(OR1200_SIM)/vmem

# OR1200 Simulation Program
OR1200_PROGRAM := $(OR1200_VMEM)/hello_world.vmem

# Check if log parameter set
ifndef LOG
	LOG := 0
else
	ifndef OUT_DIR
		OUT_DIR := logs
	endif
endif

# Check if VCD only parameter set
ifndef VCD_ONLY
	VCD_ONLY := 0
endif

# Check if DOT only parameter set
ifndef DOT_ONLY
	DOT_ONLY := 0
endif

all: tgt-ttb aes uart or1200

# Circuit Analyses
# AES
aes: output_dir aes-cdd aes-cdn aes-cnd aes-cnn aes-ddd aes-ddn aes-dnd aes-dnn aes-tjfree

# UART
uart: output_dir uart-cdd uart-cdn uart-cnd uart-cnn uart-ddd uart-ddn uart-dnd uart-dnn uart-tjfree

# OR1200
or1200: output_dir or1200-cdd or1200-cdn or1200-cnd or1200-cnn or1200-ddd or1200-ddn or1200-dnd or1200-dnn or1200-tjfree

output_dir:
	if [ $(LOG) != 0 ]; then \
		mkdir -p $(OUT_DIR); \
	fi

aes-%:
	@echo "Starting $@ analysis..."; \
	$(eval TYPE:=$(word 2, $(subst -, ,$@))) \
	if [ $(LOG) == 0 ]; then \
		$(MAKE) -C $(AES)/$(TYPE); \
	else \
		$(MAKE) -C $(AES)/$(TYPE) &> $(OUT_DIR)/$@.log; \
	fi; \
	echo "Finished $@ analysis."

uart-%:
	@echo "Starting $@ analysis..."; \
	$(eval TYPE:=$(word 2, $(subst -, ,$@))) \
	if [ $(LOG) == 0 ]; then \
		$(MAKE) -C $(UART)/$(TYPE); \
	else \
		$(MAKE) -C $(UART)/$(TYPE) &> $(OUT_DIR)/$@.log; \
	fi; \
	echo "Finished $@ analysis."

or1200-%: $(OR1200_PROGRAM)
	@echo "Starting $@ analysis..."; \
	$(eval TYPE:=$(word 2, $(subst -, ,$@))) \
	cp $(OR1200_PROGRAM) $(OR1200)/$(TYPE)/sram.vmem; \
	if [ $(LOG) == 0 ]; then \
		$(MAKE) -C $(OR1200)/$(TYPE); \
	else \
		$(MAKE) -C $(OR1200)/$(TYPE) &> $(OUT_DIR)/$@.log; \
	fi; \
	echo "Finished $@ analysis."

%.vmem:
	$(MAKE) -C $(OR1200_SIM)

# Building the TTB IVL Target Module
tgt-ttb: 
	$(MAKE) -C $(TGT_TTB_DIR)

.PHONY: clean clean-aes clean-uart clean-or1200 tgt-ttb output_dir aes uart or1200

clean: clean-aes clean-uart clean-or1200

clean-aes:
	$(MAKE) clean -C $(AES)/cdd
	$(MAKE) clean -C $(AES)/cdn
	$(MAKE) clean -C $(AES)/cnd
	$(MAKE) clean -C $(AES)/cnn
	$(MAKE) clean -C $(AES)/ddd
	$(MAKE) clean -C $(AES)/ddn
	$(MAKE) clean -C $(AES)/dnd
	$(MAKE) clean -C $(AES)/dnn
	$(MAKE) clean -C $(AES)/tjfree

clean-uart:
	$(MAKE) clean -C $(UART)/cdd
	$(MAKE) clean -C $(UART)/cdn
	$(MAKE) clean -C $(UART)/cnd
	$(MAKE) clean -C $(UART)/cnn
	$(MAKE) clean -C $(UART)/ddd
	$(MAKE) clean -C $(UART)/ddn
	$(MAKE) clean -C $(UART)/dnd
	$(MAKE) clean -C $(UART)/dnn
	$(MAKE) clean -C $(UART)/tjfree

clean-or1200:
	$(MAKE) clean -C $(OR1200)/cdd
	$(MAKE) clean -C $(OR1200)/cdn
	$(MAKE) clean -C $(OR1200)/cnd
	$(MAKE) clean -C $(OR1200)/cnn
	$(MAKE) clean -C $(OR1200)/ddd
	$(MAKE) clean -C $(OR1200)/ddn
	$(MAKE) clean -C $(OR1200)/dnd
	$(MAKE) clean -C $(OR1200)/dnn
	$(MAKE) clean -C $(OR1200)/tjfree
