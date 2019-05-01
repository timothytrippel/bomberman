# Directory Structure
TGT_TTB_DIR :=../tgt-ttb
SCRIPTS     :=../scripts
AES         := aes-t500
UART        := uart
OR1200      := or1200
SIM_VMEMS   := $(OR1200)/simulation/vmem

# OR1200 Simulation Program
OR1200_PROGRAM := hello_world

all: tgt-ttb aes uart or1200

# Circuit Analyses
# AES
aes: aes-cdd aes-cdn aes-cnd aes-cnn aes-ddd aes-ddn aes-dnd aes-dnn aes-tjfree

# UART
uart: uart-cdd uart-cdn uart-cnd uart-cnn uart-ddd uart-ddn uart-dnd uart-dnn uart-tjfree

# OR1200
or1200: or1200-cdd or1200-cdn or1200-cnd or1200-cnn or1200-ddd or1200-ddn or1200-dnd or1200-dnn or1200-tjfree

aes-%:
	@$(eval TYPE:=$(word 2, $(subst -, ,$@))) \
	$(MAKE) -C $(AES)/$(TYPE) &> $@.log && \
	echo "Finished $@ analysis."

uart-%:
	@$(eval TYPE:=$(word 2, $(subst -, ,$@))) \
	$(MAKE) -C $(UART)/$(TYPE) &> $@.log && \
	echo "Finished $@ analysis."

or1200-%: 
	@$(eval TYPE:=$(word 2, $(subst -, ,$@))) \
	cp $(SIM_VMEMS)/$(OR1200_PROGRAM).vmem $(OR1200)/$(TYPE)/sram.vmem && \
	$(MAKE) -C $(OR1200)/$(TYPE) &> $@.log && \
	echo "Finished $@ analysis."

# Building the TTB IVL Target Module
tgt-ttb: 
	$(MAKE) -C $(TGT_TTB_DIR)

.PHONY: cleanall clean clean-aes clean-uart clean-or1200 tgt-ttb aes uart or1200

cleanall: clean
	$(MAKE) cleanall -C $(TGT_TTB_DIR)

clean: clean-aes clean-uart clean-or1200
	@$(RM) *.log
	
clean-aes:
	@$(MAKE) clean -C $(AES)/cdd
	@$(MAKE) clean -C $(AES)/cdn
	@$(MAKE) clean -C $(AES)/cnd
	@$(MAKE) clean -C $(AES)/cnn
	@$(MAKE) clean -C $(AES)/ddd
	@$(MAKE) clean -C $(AES)/ddn
	@$(MAKE) clean -C $(AES)/dnd
	@$(MAKE) clean -C $(AES)/dnn
	@$(MAKE) clean -C $(AES)/tjfree

clean-uart:
	@$(MAKE) clean -C $(UART)/cdd
	@$(MAKE) clean -C $(UART)/cdn
	@$(MAKE) clean -C $(UART)/cnd
	@$(MAKE) clean -C $(UART)/cnn
	@$(MAKE) clean -C $(UART)/ddd
	@$(MAKE) clean -C $(UART)/ddn
	@$(MAKE) clean -C $(UART)/dnd
	@$(MAKE) clean -C $(UART)/dnn
	@$(MAKE) clean -C $(UART)/tjfree

clean-or1200:
	@$(MAKE) clean -C $(OR1200)/cdd
	@$(MAKE) clean -C $(OR1200)/cdn
	@$(MAKE) clean -C $(OR1200)/cnd
	@$(MAKE) clean -C $(OR1200)/cnn
	@$(MAKE) clean -C $(OR1200)/ddd
	@$(MAKE) clean -C $(OR1200)/ddn
	@$(MAKE) clean -C $(OR1200)/dnd
	@$(MAKE) clean -C $(OR1200)/dnn
	@$(MAKE) clean -C $(OR1200)/tjfree
