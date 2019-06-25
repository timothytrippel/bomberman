#!/bin/sh

BENCHMARKS='aes basicmath blowfish crc dijkstra fft helloworld limits lzfx qsort randmath rc4 rsa sha stringsearch susan syscall'

for bench in ${BENCHMARKS}; do
	echo "${bench}"
	pushd src/${bench};
	# make clean;
	make clean all;
	popd;
done
