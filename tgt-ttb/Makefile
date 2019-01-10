BASE_IVERILOG_DIR=/Users/ti27457/Repos/iverilog
EXEC_DIR=/Users/ti27457/Repos/iverilog/lib/ivl

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644

IDIR=/Users/ti27457/Repos/iverilog
CC=g++
CFLAGS=-I$(IDIR) -fpic -std=gnu++0x -Wall -Wextra -Wshadow -g -O2

TGT=ttb
SRCS=$(wildcard *.cc)
OBJS=$(SRCS:.cc=.o)

all: $(TGT) confs install

$(TGT): $(OBJS)
	$(CC) -undefined suppress -flat_namespace $(CFLAGS) -o $@.tgt $^ 

%.o: %.cc
	$(CC) $(CFLAGS) -c -o $@ $<

confs: ./generate_tgt_confs.sh
	./generate_tgt_confs.sh $(TGT)

.PHONY: install uninstall clean 

install:
	$(INSTALL_PROGRAM) ./$(TGT).tgt  "$(EXEC_DIR)/$(TGT).tgt"
	$(INSTALL_DATA)    $(TGT).conf   "$(EXEC_DIR)/$(TGT).conf"
	$(INSTALL_DATA)    $(TGT)-s.conf "$(EXEC_DIR)/$(TGT)-s.conf"

uninstall:
	rm -f $(EXEC_DIR)/$(TGT).tgt
	rm -f $(EXEC_DIR)/$(TGT).conf
	rm -f $(EXEC_DIR)/$(TGT)-s.conf

clean:
	rm -f *.o
	rm -f *.tgt
	rm -f *.conf

cleanall: clean uninstall
