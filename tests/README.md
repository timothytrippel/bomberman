Example of Using the tools:
Unit Testing Directory:
===========================

There are two sub-directories: 

1. ivl_ttb/ - This sub-directory contains unit-tests for the Icarus Verilog backend target module: tgt-ttb. Additionally their is a makefile for automating the building, and executing of each unit tests. Operate the makefile as follows:

	a. To run all unit tests, type: make all.  
	b. To run an individual unit test, type: make run-<unit test>. 
	c. To only build IVL target module, type: make build. 
	d. To remove unit test output, type: make clean. 
	e. To delete unit test output and compiled IVL backend, type: make cleanall.

2. analysis_flow/ - This sub-directory contains unit-test for the entiry TTB analysis flow: IVL TTB backend (Dot generator), TVL vvp Verilog test bench simulator (VCD generator), Python VCD parser, Python Dot parser, and Python counter analysis program/scripts. Each directory in this sub-directory contains a makefile that:
	
	a. generates a Dot file,
	b. generates a VCD file, and
	c. analyzes the Dot and VCD files to find malicious counter based trigger components.

To operate the makefile type: "make all", to execute the entire analysis flow, and "make clean", to delete all generated files.
