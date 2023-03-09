This folder contains the mode-modified version of the Gem5 simulator.

This is based on Gem5 22.0.0.2

## Building

This thesis did not touch the build process at all so building should work like normal

Generally, just execute:

`scons build/RISCV/gem5.opt -jX`

This version of Gem5 requires a fairly old version of the scons build system (Scons 3).
Use virtualenv together with python-pip if your shipped scons is too new.


## Running Code using Gem5

Gem5 requires two files: A kernel and a disk image.

The disk image needs to be placed in this folder under the name `fs.img`.
Afaik, a dummy disk image has to present even if the used kernel does not have a disk driver

The kernel needs to be placed in this folder under the name `kernel`.

Gem5 is then executed using `./build/RISCV/gem5.opt run_lupv.py CPUMODEL`.

Where CPUMODEL is either "atomic" (fast, inaccurate) or "o3" (slow, accurate).


## Interacting with xv6

To interact with the simulated xv6, use the m5term terminal located at util/term/m5term

The xv6 terminal is very twitchy. Type slowly and accurately.

Too many backspaces or typing while the simulation is stopped due to, for example, a debugging session will permanently mess up the terminal buffer and require a restart.
