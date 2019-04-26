# Directory Structure
TGT_TTB_DIR :=../tgt-ttb
SCRIPTS     :=../scripts
AES         := AES-T500
RS232       := RS-232
OR1200      := OR1200

all: tgt-ttb aes rs232

# Circuit Analyses
# AES
aes: aes-cdd aes-cdn aes-cnd aes-cnn aes-ddd aes-ddn aes-dnd aes-dnn

aes-cdd:
	$(MAKE) -C $(AES)/CDD &> $@.log
	@echo "Finished $@ analysis."

aes-cdn:	
	$(MAKE) -C $(AES)/CDN &> $@.log
	@echo "Finished $@ analysis."

aes-cnd:	
	$(MAKE) -C $(AES)/CND &> $@.log
	@echo "Finished $@ analysis."

aes-cnn:	
	$(MAKE) -C $(AES)/CNN &> $@.log
	@echo "Finished $@ analysis."

aes-ddd:	
	$(MAKE) -C $(AES)/DDD &> $@.log
	@echo "Finished $@ analysis."

aes-ddn:	
	$(MAKE) -C $(AES)/DDN &> $@.log
	@echo "Finished $@ analysis."

aes-dnd:	
	$(MAKE) -C $(AES)/DND &> $@.log
	@echo "Finished $@ analysis."

aes-dnn:	
	$(MAKE) -C $(AES)/DNN &> $@.log
	@echo "Finished $@ analysis."

aes-tjfree:
	$(MAKE) -C $(AES)/TjFree &> $@.log
	@echo "Finished $@ analysis."

# RS232
rs232: rs232-cdd rs232-cdn rs232-cnd rs232-cnn rs232-ddd rs232-ddn rs232-dnd rs232-dnn

rs232-cdd:
	$(MAKE) -C $(RS232)/CDD &> $@.log
	@echo "Finished $@ analysis."
	
rs232-cdn:
	$(MAKE) -C $(RS232)/CDN &> $@.log
	@echo "Finished $@ analysis."

rs232-cnd:
	$(MAKE) -C $(RS232)/CND &> $@.log
	@echo "Finished $@ analysis."

rs232-cnn:
	$(MAKE) -C $(RS232)/CNN &> $@.log
	@echo "Finished $@ analysis."

rs232-ddd:
	$(MAKE) -C $(RS232)/DDD &> $@.log
	@echo "Finished $@ analysis."

rs232-ddn:
	$(MAKE) -C $(RS232)/DDN &> $@.log
	@echo "Finished $@ analysis."

rs232-dnd:
	$(MAKE) -C $(RS232)/DND &> $@.log
	@echo "Finished $@ analysis."

rs232-dnn:
	$(MAKE) -C $(RS232)/DNN &> $@.log
	@echo "Finished $@ analysis."

rs232-tjfree:
	$(MAKE) -C $(RS232)/TjFree &> $@.log
	@echo "Finished $@ analysis."

# OR1200
or1200: or1200-cdd or1200-cdn or1200-cnd or1200-cnn or1200-ddd or1200-ddn or1200-dnd or1200-dnn

or1200-cdd:
	$(MAKE) -C $(OR1200)/CDD &> $@.log
	@echo "Finished $@ analysis."

or1200-cdn:	
	$(MAKE) -C $(OR1200)/CDN &> $@.log
	@echo "Finished $@ analysis."

or1200-cnd:
	$(MAKE) -C $(OR1200)/CND &> $@.log
	@echo "Finished $@ analysis."

or1200-cnn:
	$(MAKE) -C $(OR1200)/CNN &> $@.log
	@echo "Finished $@ analysis."

or1200-ddd:
	$(MAKE) -C $(OR1200)/DDD &> $@.log
	@echo "Finished $@ analysis."

or1200-ddn:
	$(MAKE) -C $(OR1200)/DDN &> $@.log
	@echo "Finished $@ analysis."

or1200-dnd:
	$(MAKE) -C $(OR1200)/DND &> $@.log
	@echo "Finished $@ analysis."

or1200-dnn:
	$(MAKE) -C $(OR1200)/DNN &> $@.log
	@echo "Finished $@ analysis."

or1200-tjfree:
	$(MAKE) -C $(OR1200)/TjFree &> $@.log
	@echo "Finished $@ analysis."

# Building the TTB IVL Target Module
tgt-ttb: 
	$(MAKE) -C $(TGT_TTB_DIR)

.PHONY: cleanall clean clean-aes clean-rs232 clean-or1200 tgt-ttb aes rs232 or1200

cleanall: clean
	$(MAKE) cleanall -C $(TGT_TTB_DIR)

clean: clean-aes clean-rs232 clean-or1200
	$(RM) -f *.log
	
clean-aes:
	$(MAKE) clean -C $(AES)/CDD
	$(MAKE) clean -C $(AES)/CDN
	$(MAKE) clean -C $(AES)/CND
	$(MAKE) clean -C $(AES)/CNN
	$(MAKE) clean -C $(AES)/DDD
	$(MAKE) clean -C $(AES)/DDN
	$(MAKE) clean -C $(AES)/DND
	$(MAKE) clean -C $(AES)/DNN
	$(MAKE) clean -C $(AES)/TjFree

clean-rs232:
	$(MAKE) clean -C $(RS232)/CDD
	$(MAKE) clean -C $(RS232)/CDN
	$(MAKE) clean -C $(RS232)/CND
	$(MAKE) clean -C $(RS232)/CNN
	$(MAKE) clean -C $(RS232)/DDD
	$(MAKE) clean -C $(RS232)/DDN
	$(MAKE) clean -C $(RS232)/DND
	$(MAKE) clean -C $(RS232)/DNN
	$(MAKE) clean -C $(RS232)/TjFree

clean-or1200:
	$(MAKE) clean -C $(OR1200)/CDD
	$(MAKE) clean -C $(OR1200)/CDN
	$(MAKE) clean -C $(OR1200)/CND
	$(MAKE) clean -C $(OR1200)/CNN
	$(MAKE) clean -C $(OR1200)/DDD
	$(MAKE) clean -C $(OR1200)/DDN
	$(MAKE) clean -C $(OR1200)/DND
	$(MAKE) clean -C $(OR1200)/DNN
	$(MAKE) clean -C $(OR1200)/TjFree
