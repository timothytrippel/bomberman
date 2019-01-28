Executing Tests:
===========================

There are two sub-directories: 

**ivl_ttb/** - This sub-directory contains tests for the Icarus Verilog backend target module: tgt-ttb. Additionally their is a makefile for automating the building, and executing of each test. Operate the makefile as follows:

  * To run all tests, type: *make all*.
  * To run an individual test, type: *make <test circuit\>.dot*.
  * To only build IVL target module, type: *make build*.
  * To remove test outputs, type: *make clean*.
  * To delete test outputs and compiled IVL backend, type: *make cleanall*.

**analysis_flow/** - This sub-directory contains unit-test for the entiry TTB analysis flow: IVL TTB backend (Dot generator), TVL vvp Verilog test bench simulator (VCD generator), Python VCD parser, Python Dot parser, and Python counter analysis program/scripts. Each directory in this sub-directory contains a makefile that:
	
  * generates a Dot file,  
  * generates a VCD file, and  
  * analyzes the Dot and VCD files to find malicious counter based trigger components.  

To operate the makefile type: *make all*, to execute the entire analysis flow, and *make clean*, to delete all generated files.
