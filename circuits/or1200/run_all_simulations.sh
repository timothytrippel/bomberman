#!/bin/sh

BENCHMARKS=(
helloworld
alignillegalinsn
backtoback_jmp
basic
cmov
cy
dsxinsn
dsx
ext
ffl1
icache
illegalinsndelayslot
illegalinsn
insnfetchalign
insnfetcherror
intloop
intmulticycle
intsyscall
inttickloop
jmp
jr
lsualigndelayslot
lsualign
lsuerrordelayslot
lsuerror
lsu
lwjr
mmu
mul
mul-basic
msync
newlibirq
ovcy
ov
regjmp
rfe
sfbf
sf
shiftopts
shortbranch
shortjump
simple
systemcall
tickloop
tickrfforward
ticksyscall
timer
trapdelayslot
trap)

for i in {47..48}; do
# for i in {13..13}; do
	rm -rf tjfree_${BENCHMARKS[i]}
	make all LOG=1 OUT_DIR=tjfree_${BENCHMARKS[i]} PROGRAM_NUM=${i}
done

# # Tests that don't finish
# alignillegalinsn
# intloop
# intmulticycle
# intsyscall
# inttickloop
# lsuerrordelayslot
# lsuerror
# msync
# sfbf - python analysis
# timer - python analysis

# # Tests that fail
# dsxinsn
# newlibirq