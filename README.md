# Bomberman

**Author:**         Timothy Trippel <br>
**Affiliation:**    University of Michigan <br>
**Last Updated:**   05/08/2020 <br>

Bomberman is a **ticking timebomb Trojan** (TTT) specific verification tool.
It identifies suspicious state-saving components (SSCs) in a hardware design
that could *potentially* be part of a TTT. Bomberman starts by assuming *all*
SSCs are suspicious, and subsequently classifies each SSC as benign if it
expresses values that violate a set of invariants during verification
simulations.

TTTs are comprised of SSCs that incrementally approach a triggering value. The
set of invariants that define TTB SSC behavior are as follows:
1. values must never be repeated without a system reset, and
2. all possible values must never be expressed without triggering the Trojan.

Bomberman leverages these two invariants while analyzing simulation results of a
hardware design to classify whether or not an SSC compromises a TTT.

Bomberman consists of two main stages as shown in Figure 1:
1. SSC Identification
2. SSC Classification
<figure>
    <p align="center">
        <img src="/figures/bomberman.png" data-canonical-src="/figures/bomberman.png" width="75%"/>
        <br>
        <b>Figure 1: Bomberman Architecture</b>
    </p>
</figure>


The **SSC Identification** stage locates all SSCs in a hardware design described
in Verilog. It is broken into two stages: 1) *Data-Flow Graph (DFG) Generation*,
and 2) *SSC Enumeration*. During *DFG Generation*, a data-flow graph describing
bit-level signal dependencies is generated from the HDL using a custom Icarus
Verilog (IVL) compiler back-end. The DFG is encoded in the Graphviz `.dot`
format, and passed to the *SSC Enumeration* stage. The *SSC Enumeration* stage
analyzes the DFG and enumerates all signals that are the direct out of SSCs.
There are two types of SSCs that are identified: *coalesced* and *distributed*.
This list of SSCs is then passed to the *SSC Classification* for further
processing.

The **SSC Classification** stage analyzes verification simulations, in the
*value change dump* (VCD) format, to check if any SSC violates either invariant
(above) that indicates it is benign. SSCs that do not violate either invariants
are reported to the user (in the form of JSON files) to be further manually
scrutinized.

# Directory Structure

### Circuits

The `circuits/` directory contains three example hardware designs to try out the
Bomberman toolchain on. These designs are: a 128-bit AES accelerator (CNTR mode
only), a UART core, and an OR1200 processor. For information on how to run the
Bomberman toolchain on each hardware design, see the **E2E Bomberman Analysis of
Real Circuits** section below.

### Scripts

The `scripts/` directory contains the Python scripts that implement the
**Simulation Analysis** stage (Figure 1). The main script is contained in
`analyze.py`.

### Tests

The `tests/` directory contains regression tests for: 1) the entire Bomberman
analysis flow, contained in the `analysis_flow` sub-directory, and 2) the IVL
back-end HDL data-flow graph generator, contained in the `ivl_ttb`
sub-directory.

The `analysis_flow` sub-directory contains three hardware designs and
corresponding test benches to exercise the Bomberman toolchain. See the
*Testing* section below for how to test bomberman on these designs.

The `ivl_ttb` sub-directory contains 62 regression tests (i.e., hardware
designs) to exercise the IVL compiler back-end data-flow graph generator and
verify its correctness. See the *Testing* section below for how to execute these
tests.

### TGT-TTB

The `tgt-ttb/` directory contains the IVL compiler back-end data-flow graph
generator that implements the *SSC Identification* stage.

# Installation

### 1. Repository Cloning

You can clone the repository using:

`git clone https://github.com/mit-ll/ttb.git`

### 2. Initialize Git Submodules

Bomberman utilizes [Icarus Verilog](https://github.com/steveicarus/iverilog)
(IVL) as a submodule. Thus, the initialization of the IVL submodule must also be
done as follows:

```
cd ttb/
git submodule update --init --recursive
```

### 3. Disabling optimization functions of IVL

Disabling the optimization functions of IVL is important for preserving the
input netlists structure as-is for analysis by Bomberman. To do so, you must
**comment out** two blocks of code in the `ttb/iverilog/main.cc` file in the
top-level IVL source code (lines 1251 and 1254-1260) submodule directory as
follows:

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

***NOTE:** make sure to cross-reference the line numbers with the code snippets
above since the IVL code base is actively managed and the exact line numbers are
subject to change.*

### 4. Building IVL

To build IVL from source requires the following dependencies:

`autoconf`
`gperf`
`flex`
`bison`

On MacOS these can be installed with homebrew using: `brew install <package>`.
After installing the dependencies, proceed to build IVL as follows:

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

To re-compile and re-install into IVL after modifications, replace step 2
(above) with: `make cleanall all`.

# Testing

### 1. Data-Flow Graph Generator

There are 62 regression tests to verify the correctness of the data-flow graph
generator contained in the `tgt-ttb` IVL back-end target module. Each regression
test consists of a small circuit, described in Verilog, that exercises the
data-flow graph generator's ability to handle various Verilog syntax and
expressions. All 62 regression tests should run, but only 61 tests should pass.
This is due to a minor (known) error in the way the graph-generator handles
duplicate signals in a concatenation, and will be fixed in a future release.

Each regression test generates a .dot file describing a data-flow graph from a
simple test circuit. The resulting graph is automatically checked for
correctness against provided *gold* examples. Additionally a PDF respresentation
is generated for manual inspection of correctness, but this *requires Graphviz
to be installed.*

To run all 62 regression tests use:

1. Install Graphviz: `brew install graphviz`
1. `cd tests/ivl_ttb`
2. `make test`
3. `cd ..`
4. `cd ..`

### 2. Bomberman E2E Analysis

T here are 3 regression tests to verify the correctness of the entire Bomberman
toolchain, from the data-flow graph generator to the simulation analysis scripts
(Figure 1). Each regression tests is comprised of a small circuit design, and an
associated test bench that excerises the design. The three circuits are: a
simple counter (`counter/`), a D flip-flop (`d_ff/`), and a simple combinational
circuit (`split/`).

***Note:*** *Bomberman's simulation analysis requires Python 2.7. You may have
to modify the makefile (tests/analysis_flow/Makefile) to invoke Python 2.7, if
the **python** alias on your system is mapped to Python 3.*

To run all 3 regression tests use:

1. `cd tests/analysis_flow`
2. `make all`
3. `cd ..`
4. `cd ..`

To run only a single regression test use: `make <design>`, where `<design>` is
either `counter`, `d_ff`, or `split`.

# HDL Data-flow Graph Generation

Bomberman's data-flow graph generator (DFGG) can be utilized independently of
Bomberman for performing various static analyses of a circuit's HDL. The DFGG is
simply an Icarus Verilog (IVL) compiler back-end that generates data-flow graphs
in the [Graphviz](https://www.graphviz.org/) `.dot` format. The DFGG takes the
following as input:

|     | Input                  | Type             | Default    |
| --- | ---------------------- | ---------------- | ---------- |
|  1  | Clock Signal Basename  | string           | n/a        |
|  2  | Verilog Source Files   | file name(s)     | n/a        |
|  3  | Dot Output File Name   | string           | n/a        |

The DFGG can be invoked from the root repository directory (ttb/) using:

`iverilog/iverilog -o <dot output file name> -pclk=<clock basename> -t ttb
<verilog source file> ...`

# E2E Bomberman Analysis of Real Circuits

There are three real-world circuit designs provided within this repository to
experiment with. These designs include: a 128-bit AES accelerator
([TrustHub](https://trust-hub.org/home)), an 8-bit UART module
([OpenCores](https://opencores.org/projects/uart16550)), and an OR1200 processor
CPU ([OpenCores](https://opencores.org/projects/uart16550)). To experiment
analyzing each design with Bomberman, follow the steps below.

***Note:*** *Bomberman's simulation analysis requires Python 2.7. You may have
to modify the makefile (circuits/common.mk) to invoke Python 2.7, if the
**python** alias on your system is mapped to Python 3. Alternatively, the
makefile can be altered to invoke [PyPy](https://pypy.org/), instead of Python
2.7, to speed up computation. If you go this route, be sure to install PyPy on
your system before proceeding.*

1.  `cd circuits`
2.  `cd aes`
3.  `make all LOG=1 OUT_DIR=output`
4.  `cd ..`
5.  `cd uart`
6.  `make all LOG=1 OUT_DIR=output`
7.  `cd ..`
8.  `cd or1200`
9.  `make all LOG=1 OUT_DIR=output`
10. `cd ..`

The master Makefile (`circuits/common.mk`) that is invoked by running the above
commands in each design sub-directory does three things. First, it invokes the
*data-flow graph generator*, which generates a `.dot` file encoding the
data-flow graph for the given hardware design. Second, *IVL* is invoked to
simulate the hardware design and generate a `.vcd` file encoding the simulation
trace. Third, the *simulation analysis* script is invoked to analyze the design
for suspicious SSCs. The number of suspicious SSCs computed at different points
throughout the simulation are output into several `.json` files.

There are several Jupyter Notebooks for plotting the results encoded in each
`.json` file in the `circuits/plots` directory. Additionally, there are several
scripts for running the Bomberman analysis of each design on a SLURM managed
cluster, if more compute power is needed. However, each of the 3 provided
designs has been tested on a 15in Macbook Pro with a 3.1 GHz Intel Core i7
processor and 16GB of DDR3 RAM, and each Bomberman analysis took less then a
couple minutes to run (including simulation time).


# License

Copyright (c) 2019, Massachusetts Institute of Technology.

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Distribution Statement

DISTRIBUTION STATEMENT A. Approved for public release. Distribution is unlimited.

This material is based upon work supported by the Under Secretary of Defense for
Research and Engineering under Air Force Contract No. FA8702-15-D-0001. Any
opinions, findings, conclusions or recommendations expressed in this material
are those of the author(s) and do not necessarily reflect the views of the Under
Secretary of Defense for Research and Engineering.

© 2019 Massachusetts Institute of Technology.

MIT Proprietary, Subject to FAR52.227-11 Patent Rights - Ownership by the
contractor (May 2014)

The software/firmware is provided to you on an As-Is basis

Delivered to the U.S. Government with Unlimited Rights, as defined in DFARS Part
252.227-7013 or 7014 (Feb 2014). Notwithstanding any copyright notice, U.S.
Government rights in this work are defined by DFARS 252.227-7013 or DFARS
252.227-7014 as detailed above. Use of this work other than as specifically
authorized by the U.S. Government may violate any copyrights that exist in this
work.
