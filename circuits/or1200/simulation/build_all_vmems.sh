#!/bin/sh

# BENCHMARKS='aes basicmath bitcount crc'
BENCHMARKS='aes basicmath crc'

for bench in ${BENCHMARKS}; do
	echo "${bench}"
	pushd src/${bench};
	make clean all;
	popd;
done
