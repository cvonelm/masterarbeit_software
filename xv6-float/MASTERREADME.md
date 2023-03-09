This folder contains the modified mode-based xv6 operating system as well as the application benchmarks

## Folder structure

kernel/ contains the kernel code. Most importantly, the file mode.h and mode.c contain most of the mode implementation, although the implementation is scattered all over the kernel code.

user/ contains the xv6 applications (mostly very basic implementations of UNIX tools) and the application benchmarks user/unsecure.c contains the unsecured variant, user/secure.c contains the mode-secured variant and user/procsec.c contains the process-secured variant

mkfs/ contains the tool to generate the disk image in xv6s proprietary disk format.

## BUILDING

Requirements:

- RISC-V cross compiler suite (riscv64-elf-gcc on Arch, gcc-riscv64-unknown-elf on Debian)
- the patched binutils that is also part of this repository
- a RISC-V newlib (riscv64-elf-newlib on Arch)
- Common development tools such as make etc.


## Building the kernel:

Invoke `make`, this will build the kernel, which is placed at kernel/kernel

## Building the application benchmarks:

Because of the harsh size restrictions the xv6 filesystem imposes on us, this is split into three distinct disk images:

`make unsecure.img` builds the unsecure variant
`make secure.img` builds the mode-secured variant
`make procsec.img` builds the process secured variant

All of these generate a fs.img disg image file.

The applications are then named `unsecuresql`, `securesql`, `procsecsql` respectively.

## Running the application benchmarks

Explained in the Gem5 README.
