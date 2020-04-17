#/bin/bash

make clean
make test_vcd
rm -rf output
mkdir output
cp testbench.vvp output/testbench.vvp
make LOG=1 OUT_DIR=output output/testbench.dot
make LOG=1 OUT_DIR=output output/testbench.vcd
cp testbench.vcd output/testbench.vcd
make LOG=1 OUT_DIR=output script
