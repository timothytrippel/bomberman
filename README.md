# Bomberman

**Author:**         Timothy Trippel <br>
**Email:**          timothy.trippel@ll.mit.edu <br>
**Last Updated:**   8/20/2019 <br>

Bomberman is a **ticking timebomb** (TTB) Trojan specific verification tool. It indentifies suspicious state-saving components (SSCs) in a hardware design that could *potentially* be part of a TTB Trojan. Bomberman starts by assuming *all* SSCs are suspicious, and subsequently classifies each SSC as benign if it expresses values that violate a set of invariants during verification simulations. 

As detailed in our [technical report](technical_report.pdf), TTBs are comprised of SSCs that incrementally approach a triggering value. The set of invariants that define TTB SSC behavior are as follows:
1. values must never be repeated without a system reset, and 
2. all possible values must never be expressed without triggering the Trojan.

Bomberman leverages these two invariants while anlayzing simulation results of a hardware design to classify whether or not an SSC is part of a TTB Trojan.

Bomberman consists of two main stages as shown in Figure 1:
1. SSC Indentification
2. Simulation Analysis
<figure>
    <p align="center">
        <img src="/figures/bomberman.png" data-canonical-src="/figures/bomberman.png" width="75%"/>
        <br>
        <b>Figure 1: Bomberman Architecture</b>
    </p>
</figure>


The **SSC Identification** stage locates all SSCs in a hardware design described in Verilog. It does so by first generating a data-flow graph from the HDL using a custom the Icarus Verilog (IVL) compiler back-end. The data-flow graph is encoded in the Graphviz `.dot` format, and passed to the next stage.

The **Simulation Analysis** stage parses: 1) the `.dot` file encoding the circuit data-flow graph generated by the *SSC Identification*, and 2) the verification simulation results, in the Value Change Dump (`.vcd`) format. First, during *SSC Enumeration*, it enumerates all SSCs in the data-flow graph, marking each SSC as *suspicious*. There are two types of SSCs that are identified: *coalesced* and *distributed*. For detailed information on the differences between the two types of SSCs, please refer to the [technical report](technical_report.pdf). Next, during *Invariant Testing*, the values expressed by each SSC over the course of the simulation timeline are extracted from the simulation results. SSCs that violate either of the two invariants listed above are marked as *benign*.

# Directory Structure

### Circuits

The `circuits/` directory contains three example hardware designs to try out the Bomberman toolchain on. These designs are: an 128-bit AES accelerator (CNTR mode only), a UART core, and an OR1200 processor. For information on how to run the Bomberman toolchain on each hardware design, see the README.md file in the the `circuits/` directory.

### Scripts

The `scripts/` directory contains the Python scripts that implement the **Simulation Analysis** stage (Figure 1). The main script is contained in `analyze.py`.

### Tests

The `tests/` directory contains regression tests for: 1) the entire Bomberman analysis flow, contained in the `analysis_flow` sub-directory, and 2) the IVL back-end HDL data-flow graph generator, contained in the `ivl_ttb` sub-directory. 

The `analysis_flow` sub-directory contains three hardware designs and corresponding test benches to exercise the Bomberman toolchain. See the *Testing* section below for how to test bomberman on these designs.

The `ivl_ttb` sub-directory contains 62 regression tests (i.e., hardware designs) to exercise the IVL compiler back-end data-flow graph generator and verify its correctness. See the *Testing* section below for how to execute these tests.

### TGT-TTB

The `tgt-ttb/` directory contains the IVL compiler back-end data-flow graph generator that implements the *SSC Identification* stage.

# Installation

### 1. Repository Cloning

You can clone the repository using:

`git clone https://llcad-github.llan.ll.mit.edu/HSS/ttb.git`

### 2. Initialize Git Submodules

Bomberman utilizes [Icarus Verilog](https://github.com/steveicarus/iverilog) as a submodule. Thus, the initialization of the Icarus Verilog submodule must also be done as follows:

```
cd ttb/
git submodule update --init --recursive
```

### 3. Disabling optimization functions of IVL

Disabling the optimization functions of IVL is important for preserving the
input netlists structure as-is for analysis by Bomberman. To do so, you must comment out two blocks of code in the `ttb/iverilog/main.cc` file in the top-level IVL source code (lines 1179, and 1182-1188) submodule directory as follows:

Line 1244:

```cout << "RUNNING FUNCTORS" << endl;```

Line 1247--1253:
```
while (!net_func_queue.empty()) {
    net_func func = net_func_queue.front();
    net_func_queue.pop();
    if (verbose_flag)
        cerr<<" -F "<<net_func_to_name(func)<< " ..." <<endl;
    func(des);
}
```

### 4. Building IVL

1. `cd iverilog`
2. comment out IVL optimization functions (see above)
3. `sh autoconf.sh`
4. `./configure --prefix=$(pwd)` 
5. `make install` 
6. `cd ..`

### 5. Building Data-Flow Graph Generator (tgt-ttb)

To compile and install into IVL for the first time:

1. `cd tgt-ttb`
2. `make all`
3. `cd ..`

To re-compile and re-install into IVL after modifications:

1. `cd tgt-ttb`
2. `make cleanall all`
3. `cd ..`

# Testing

### 1. Data-Flow Graph Generation

1. `cd tgt-ttb`
2. `make all`
3. `cd ..`


### 2. Bomberman E2E Analysis

# Usage

### 1. Generating Data-Flow Graphs
### 2. Running IVL Simulations
### 3. Analyzing Simulations
### 4. Bomberman E2E Analysis




<!-- 




![Alt text][image] 
A reference to the [image](#image).

 that cause 
enumerates all state-saving components, i.e., registers, in hardware designs bproduces a data-flow graph
To make detecting distributed counter-registers practical, we use
connection information from the circuit in question to limit the
register combinations checked by our existing analysis flow. We
represent connection information as a dataflow graph. Creating a
dataflow graph for a circuit requires parsing the textual description
of the circuit and connecting the individual assignments to form a
graph. To maximize compatibility and reduce engineering effort, we use
Icarus Verilog to parse circuits described in Verilog. Once parsed we
walk the parse tree to build a dataflow graph for the
circuit. Finally, we use the dataflow graph to determine all possible
combinations of registers in a design. The resulting combinations feed
directly into our existing flow for detecting coalesced counter
registers.

Icarus Verilog supports adding functionality via modules called
backends. Backends interact with Icarus Verilog via the backend dll
API. Using the API means that we should be able to build our backend
(tgt-ttb) independently from Icarus Verilog, but for now we need to
compile our backend and Icarus Verilog together.

# Checkout

Icarus Verilog is a submodule in our project repository. This makes it
easy for us to track with the main Icarus Verilog repository. Using
submodules requires that after cloning the project repository, we
must checkout Icarus Verilog using,

git submodule init
git submodule update

After that, every time you pull, you must run the command below to
update the Icarus Verilog submodule.

git submodule update

Building
========
As mentioned above, we build our dll backend (tgt-ttb) with iverilog currently.
Though tgt-ttb is a dll, we still have to recompile all of Icarus Verilog
since I have not been able to decouple the backend build from the rest of 
Icarus Verilog.

To do this, in the iverilog folder run

./autoconf.sh
./configure
make
sudo make install

Running C Code
==============
Compile:
or1k-elf-gcc -Wall -mnewlib -mboard=ml509 -o <output> <input ...>

Turn to Bin:
or1k-elf-objcopy -O binary <input (output from compile)> <output.bin>

Turn to vmem
~/spqr/or1200toolchain/utils/bin2vmem <input.bin> > output.vmem

Workflow (an example is given in the examples folder)
========
This project consists of a couple little tools. They all have a certain job 
and works in the following steps

1. Use the tgt-ttb Icarus Verilog backend to create a dependency graph of the 
   signals in the verilog file(s). This graph will be in the form of a dot
   file.
   $ iverilog -t ttb [verilog input file(s)] -o [output file]

2. To make it easy for our existing analysis flow to track the results
   of distributed counter analysis, we create explicit registers in
   the circuit. To automatically generate the Verilog describing the
   distributed registers, use scripts/parse_dot.py to convert the dot
   file into verilog code.

   $ python scripts/parse_dot.py [file.dot] > output.slice

3. Manually add those assign statments to your top level module.

4. To find coalesced counter registers, we first simulate a circuit. Use 
   Icarus Verilog to create a simulator for your code

   $ iverilog -t vvp -o <output file> <verilog input file(s)>

5. We then run the simulated circuit to create the VCD file.

   $ vvp [output file from step 4]

6. Using the values in the VCD file, we apply our rule-set, cycle-by-cycle 
   against the VCD file. We report what registers remain after analysis as
   possible counter-based trigger registers.
   
   $ python scripts/VCD_parse.py [vcd output file from step 5]
 -->