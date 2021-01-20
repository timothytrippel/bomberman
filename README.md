# Bomberman
<figure>
  <p align="center">
    <img src="/figures/bomberman-logo.png" data-canonical-src="/figures/bomberman-logo.png" width="50%"/>
  </p>
</figure>

Bomberman is a **ticking timebomb Trojan** (TTT) specific verification tool that
identifies suspicious state-saving components (SSCs) in a hardware design
that could *potentially* be part of a TTT. Bomberman starts by assuming *all*
SSCs are suspicious, and subsequently classifies each SSC as benign if it
expresses values that violate a set of TTT-specific invariants (below) during
verification simulations. Specifically, the set of invariants that define TTB
SSC behavior are:
1. values must never be repeated without a system reset, and
2. all possible values must never be expressed without triggering the Trojan.

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
analyzes the DFG and enumerates all signals that are the direct output of SSCs.
There are two types of SSCs that are identified: *coalesced* and *distributed*.
This list of SSCs is then passed to the *SSC Classification* for further
processing.

The **SSC Classification** stage analyzes verification simulations, in the
*value change dump* (VCD) format, to check if any SSC violates either invariant
(above) that indicates it is benign. SSCs that do not violate either invariant
are reported to the user (in the form of JSON files) to be further manually
scrutinized.

# Technical Paper

A [technical paper](https://rtcl.eecs.umich.edu/rtclweb/assets/publications/2021/bomberman-oakland-21.pdf)
describing Bomberman has been accepted to appear at the **2021
IEEE Symposium on Security and Privacy**.

# Quickstart

We provide a Dockerfile and four example circuits to demonstrate Bomberman on.
To use it, follow the steps below to build the Docker image 
(containing all required tools and dependencies) and run Bomberman in a
container. The four example circuits we provide to demonstrate Bomberman on are:
a 128-bit AES accelerator ([TrustHub](https://trust-hub.org/home)), an 8-bit
UART module ([OpenCores](https://opencores.org/projects/uart16550)), an
OR1200 CPU ([OpenCores](https://opencores.org/projects/uart16550)),
and a [PicoRV32](https://github.com/cliffordwolf/picorv32) CPU. 


### 1. Clone Repository

`git clone https://github.com/timothytrippel/bomberman.git`

### 2. Build Docker Image

1. `cd bomberman`
2. [install Docker](https://docs.docker.com/get-docker/) 
3. [add your user to the `docker` group](https://docs.docker.com/engine/install/linux-postinstall/)
4. `make build-infra`
5. grab a cup of coffee, step 4 could take a while :)

*Note: if you would like to setup your own environment to use our toolchain,
take a look at the Dockerfile for all required dependencies.*

### 3. Run Bomberman on Four Circuits in a Docker Container

`make all`

The above command invokes the `analyze_all.sh` shell script within a container.
This script navigates to each circuit directory (e.g., `circuits/<circuit>`),
and invokes a master Makefile: `circuits/common.mk`. This Makefile does three
things. First, it executes the *data-flow graph generator*, which generates a
`.dot` file encoding the data-flow graph for the given hardware design. 
Second, *IVL* is invoked to simulate the hardware design and generate a `.vcd`
file encoding the simulation trace. Third, the *Bomberman* script is 
invoked to: 1) *Enumerate* all SSCs in the target hardware design, and 
2) *Classify* each SSC as suspicious or benign. 
Lastly, the number of suspicious SSCs computed at different points
throughout the simulation are output into several `.json` files.

# Directory Structure

### Circuits

The `circuits/` directory contains test bench harnesses and Makefiles for
four example hardware designs to test Bomberman on. These designs are: a 
128-bit AES accelerator (CNTR mode only), a UART core, an OR1200 CPU, and the 
PicoRV32 RISC-V CPU (see above).

### Bomberman

The `bomberman/` directory contains the Python scripts that implement the
**SSC Enumeration** and **SSC Classification** stages (Figure 1). The main 
script is contained in `bomberman.py`.

### Tests
The `bomberman` sub-directory contains three hardware designs and
corresponding test benches to exercise the Bomberman toolchain. See the
*Testing* section below for how to test bomberman on these designs.

The `tgt-dfg` sub-directory contains 62 regression tests (i.e., hardware
designs) to exercise the IVL compiler back-end data-flow graph generator and
verify its correctness. See the *Testing* section below for how to execute these
tests.

### TGT-DFG (Data-Flow Graph Generator)

The `tgt-dfg/` directory contains the IVL compiler back-end data-flow graph
generator that implements part of the *SSC Identification* stage.

### Third Party

The `third_party` directory contains the RTL and supporting software for the 
four example circuits we provide (above) to demonstrate Bomberman's utility.

# Development in the Container

In our project-wide Makefile (in the root project directory) we include a target
to launch a shell within a container and mount source directories into the
container to jumpstart development. To use this target simply run (after the
Docker image has been built):

`make dev`

# Testing 

### 1. Data-Flow Graph Generator

There are 62 regression tests to verify the correctness of the data-flow graph
generator contained in the `tgt-dfg` IVL back-end target module. Each regression
test consists of a small circuit, described in Verilog, that exercises the
data-flow graph generator's ability to handle various Verilog syntax and
expressions. All 62 regression tests should run, but only 61 tests should pass.
This is due to a minor (known) error in the way the graph-generator handles
duplicate signals in a concatenation, and will be fixed in a future release.

Each regression test generates a .dot file describing a data-flow graph from a
simple test circuit. The resulting graph is automatically checked for
correctness against provided *gold* examples. Additionally a PDF respresentation
is generated for manual inspection of correctness, but this *requires Graphviz
to be installed.* If you run these tests within the provide Docker image, this
dependency is already installed.

To run all 62 regression tests run:

1. `make dev` (launch shell in container)
2. `cd tgt-dfg && make && cd ..` (compile and install tgt-dfg)
3. `cd tests/tgt-dfg && make test`

### 2. Bomberman E2E Testing

There are three test circuits to verify the correctness of the entire Bomberman
toolchain, from the data-flow graph generator to the simulation analysis scripts
(Figure 1). Each test is comprised of a small circuit design, and an
associated test bench that excerises the design. The three circuits are: a
simple counter (`counter/`), a D flip-flop (`d_ff/`), and a simple combinational
circuit (`split/`).

To run all three tests run:

1. `make dev` (launch shell in container, if not launched already)
2. `cd tests/bomberman`
3. `make all`

To run only a single regression test use: `make <design>`, where `<design>` is
either `counter`, `d_ff`, or `split`.

# HDL Data-flow Graph Generation

Bomberman's data-flow graph generator (DFGG) can be utilized independently of
Bomberman for performing various static analyses of a circuit's HDL. The DFGG is
simply an Icarus Verilog (IVL) compiler back-end that generates data-flow graphs
in the [Graphviz](https://www.graphviz.org/) `.dot` format. For an example of 
how to invoke it, check out the `circuits/common.mk` Makefile.

# Plotting Bomberman Results

There are several Jupyter Notebooks for plotting the output of Bomberman 
(encoded in `.json` files). The notebooks are located in the `circuits/plots`
directory. These can be used to reproduce plots from the technical paper 
(linked above).

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
