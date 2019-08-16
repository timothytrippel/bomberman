# Bomberman

Bomberman is a **ticking timebomb** (TTB) Trojan specific verification tool. It indentifies suspicious state-saving components (SSCs) in a hardware design that could *potentially* be part of a TTB Trojan. Bomberman starts by assuming *all* SSCs are suspicious, and subsequently classifies each SSC as benign if it expresses values that violate a set of invariants during verification simulations. 

As detailed in our technical report **LINK TO PAPER**, TTBs are comprised of SSCs that incrementally approach a triggering value. The set of invariants that define TTB SSC behavior are as follows:
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


The **SSC Identification** stage locates all SSCs in a hardware design described in Verilog. It does so by first generating a data-flow graph from the HDL using a custom the Icarus Verilog compiler back-end. The data-flow graph is encoded in the Graphviz `.dot` format. Next, a python script parses the `.dot` file and locates all SSCs. All SSCs are initially marked as *suspicious* There are two types of SSCs that are identified: *coalesced* and *distributed*. For detailed information on the differences between the two types of SSCs, please refer to the technical research paper **LINK TO PAPER**.

The **Simulation Analysis** stage parses simulation results, in the Value Change Dump (`.vcd`) format, and analyzes the values expressed by each SSC over the course of the simulation timeline. SSCs that violate either of the two invariants listed above are marked as *benign*

## Directory Structure

## Installation

### 1. Repository Cloning

### 2. Initialize Git Submodules

### 3. Building Icarus Verilog (IVL)

### 4. Building Data-Flow Graph Generator (tgt-ttb)

## Testing

### 1. Data-Flow Graph Generation
### 2. Bomberman E2E Analysis

## Usage

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