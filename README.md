# Bomberman

Bomberman is a *ticking timebomb* (TTB) Trojan specific verification tool. It indentifies suspicious state-saving components (SSCs) in a hardware design that could potentially be part of a TTB Trojan. Bomberman starts by assuming *all* SSCs are suspicious, and subsequently classifies each SSC as non-suspicious if it expresses values that violate a set of invariants during verification simulations. Bomberman consists of two main stages as shown in Figure 

<figure>
    <p align="center">
        <img src="/figures/bomberman.png" data-canonical-src="/figures/bomberman.png" width="75%"/>
        <br>
        <b>Figure 1: Bomberman Architecture</b>
    </p>
</figure>

1. SSC Indentification
2. SSC Analysis

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