This folder contains the microbenchmarks.

As the prefix "xv6-" implies, all of these are based on extremely stripped down xv6 kernels.

The m5ops "resetstats" and "dumpstats" are used in all of these microbenchmarks to measure time.

m5out/stats.txt will then contain the stats.
The measurements scripts in results/ show how the stats.txt are processed.

## Building

Requirements:
- RISC-V GCC toolchain (gcc-riscv64-unknown-elf on Debian)

Execute `make` in the folder of the microbenchmark.

This will generate a kernel at kernel/kernel.
How to run the kernel is explained in the gem5/ folder.

I think that gem5 requires a dummy fs.img even though none of the microbenchmarks have any device drivers.
Just use one from the application benchmarks

## xv6-fcalltest

This simulates the empty function call.

## xv6-ccallhit

This simulates an empty child call

## xv6-ccallmiss

This simulates an empty child call with a cache miss

## xv6-pcalltest

This simulates an empty parent call

## xv6-ptableswap

This simulates the page table swap protection mechanism

This has to be run under an unmodified gem5 in gem5-unmodified/ !

## xv6-syscalltest

This simulates an empty system call.

This has to be run under the unmodified gem5 in gem5-unmodified/ !

## xv6-ecallfullswith

This simulates the full process switch

This has to be run under the unmodified gem5 in gem5-unmodified/ !

## Measuring different latencies

To change the latency of the mlbOps you have to change the following lines in Gem5

in src/cpu/o3/FuncUnitConfig.py:

     opList = [ OpDesc(opClass='IntAlu'), OpDesc(opClass="mlb", opLat=1)]

The opLat parameter.

An in src/cpu/o3/BaseO3CPU.py:

     trapLatency = Param.Cycles(13, "Trap latency")

The Trap Latency parameter.
