#!/bin/sh

BENCHMARKS='aes basicmath blowfish crc dijkstra fft limits lzfx qsort randmath rc4 rsa sha stringsearch susan'

for bench in ${BENCHMARKS}; do
	echo "${bench}"
	pushd src/${bench};
	make clean all;
	popd;
done
