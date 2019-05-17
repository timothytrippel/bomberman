#!/bin/sh

# BENCHMARKS='crc bitcount aes'
BENCHMARKS='aes'

for bench in ${BENCHMARKS}; do
	echo "${bench}"
	pushd src/${bench};
	make clean all;
	popd;
done
