#!/bin/sh

DATA_SEEDS=(
00
f6
c0
1b
a8
90
8c
1c
5b
8b
2f
a8
26
70
e0
0f
47
53
b5
96
b4
9d
28
10
68
)

for i in {0..24}; do
	rm -rf tjfree_16bytes_seed${i}
	make all LOG=1 OUT_DIR=tjfree_16bytes_seed${i} UART_DATA_SEED=${DATA_SEEDS[i]}
done