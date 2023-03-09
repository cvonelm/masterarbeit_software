This is the top-level README. The different parts of this repository, such as Gem5 and xv6 all come with their own README

# Folder Structure
- gem5/ contains the Gem5 simulator with the mode-based RISC-V processor
- gem5-unmodified/ contains the unmodified Gem5 simulator, used during some of the microbenchmarks
- binutils-softmode/ contains a version of binutils modified so you can use the assembler using the new mode instructions, such as `mepcload` etc.
- binutils-softmode-pkg/ contains an Arch package for the above modified binutils.
- xv6-float/ contains the mode-based xv6 operating system and the application benchmarks
- microbenchmarks/ contains the microbenchmarks
- riscv-opcodes/ contains an useful tool, which automatically generates the data structures binutils needs for instruction specifications from a simple text format
- prepare.py generates the `test.db` used in the application benchmarks. However, the application benchmarks already ship with a prepared `test.db`
- results/ contains the scripts for generating the graphics
