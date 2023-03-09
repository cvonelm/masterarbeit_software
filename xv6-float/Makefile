K=kernel
U=user

OBJS = \
  $K/entry.o \
  $K/start.o \
  $K/console.o \
  $K/printf.o \
  $K/tty.o \
  $K/kalloc.o \
  $K/spinlock.o \
  $K/string.o \
  $K/main.o \
  $K/kernelvec.o \
  $K/vm.o \
  $K/proc.o \
  $K/swtch.o \
  $K/trap.o \
  $K/mode.o \
  $K/syscall.o \
  $K/sysproc.o \
  $K/bio.o \
  $K/fs.o \
  $K/log.o \
  $K/sleeplock.o \
  $K/file.o \
  $K/pipe.o \
  $K/exec.o \
  $K/sysfile.o \
  $K/pic.o \
  $K/blk.o \
  $K/sys.o \
  $K/random.o \
  $K/rng.o \
  $K/rtc.o \
  $K/tmr.o


# riscv64-unknown-elf- or riscv64-linux-gnu-
# perhaps in /opt/riscv/bin
TOOLPREFIX = riscv64-elf-

# Try to infer the correct TOOLPREFIX if not set
ifndef TOOLPREFIX
TOOLPREFIX := $(shell if riscv64-unknown-elf-objdump -i 2>&1 | grep 'elf64-big' >/dev/null 2>&1; \
	then echo 'riscv64-unknown-elf-'; \
	elif riscv64-linux-gnu-objdump -i 2>&1 | grep 'elf64-big' >/dev/null 2>&1; \
	then echo 'riscv64-linux-gnu-'; \
	elif riscv64-unknown-linux-gnu-objdump -i 2>&1 | grep 'elf64-big' >/dev/null 2>&1; \
	then echo 'riscv64-unknown-linux-gnu-'; \
	else echo "***" 1>&2; \
	echo "*** Error: Couldn't find a riscv64 version of GCC/binutils." 1>&2; \
	echo "*** To turn off this error, run 'gmake TOOLPREFIX= ...'." 1>&2; \
	echo "***" 1>&2; exit 1; fi)
endif

CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)gas
LD = $(TOOLPREFIX)ld

OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump
STRIP = $(TOOLPREFIX)strip

CFLAGS = -Wall -Werror -O -fno-omit-frame-pointer -ggdb -gdwarf-2
CFLAGS += -MD
CFLAGS += -mcmodel=medany
CFLAGS += -ffreestanding -fno-common -nostdlib -mno-relax
CFLAGS += -I.
CFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)
CFLAGS += $(shell $(CC) -Wno-infinite-recursion -E -x c /dev/null >/dev/null 2>&1 && echo -Wno-infinite-recursion)

CFLAGS2 = -Wall -Werror -O0 -fno-omit-frame-pointer -ggdb -gdwarf-2
# Disable support for hardware floating-points
CFLAGS2 += -MD
CFLAGS2 += -mcmodel=medany
CFLAGS2 += -ffreestanding -fno-common -nostdlib -mno-relax
CFLAGS2 += -I.
CFLAGS2 += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)
CFLAGS2 += $(shell $(CC) -Wno-infinite-recursion -E -x c /dev/null >/dev/null 2>&1 && echo -Wno-infinite-recursion)
# Disable PIE when possible (for Ubuntu 16.10 toolchain)
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]no-pie'),)
CFLAGS += -fno-pie -no-pie
endif
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]nopie'),)
CFLAGS += -fno-pie -nopie
endif
# Disable PIE when possible (for Ubuntu 16.10 toolchain)
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]no-pie'),)
CFLAGS2 += -fno-pie -no-pie
endif
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]nopie'),)
CFLAGS2 += -fno-pie -nopie
endif

LDFLAGS = -z max-page-size=4096

$K/kernel: $(OBJS) kernel/proc.o $K/kernel.ld $U/initcode
	$(LD) $(LDFLAGS) -T $K/kernel.ld -o $K/kernel $(OBJS) 
	$(OBJDUMP) -S $K/kernel > $K/kernel.asm
	$(OBJDUMP) -t $K/kernel | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $K/kernel.sym
	$(OBJCOPY) -O binary $K/kernel $K/kernel.bin

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel/proc.o: 
	$(CC) $(CFLAGS2) -c kernel/proc.c -o $@

$U/initcode: $U/initcode.S
	$(CC) $(CFLAGS) -nostdinc -I. -Ikernel -c $U/initcode.S -o $U/initcode.o
	$(LD) $(LDFLAGS) -N -e start -Ttext 0 -o $U/initcode.out $U/initcode.o
	$(OBJCOPY) -S -O binary $U/initcode.out $U/initcode
	$(OBJDUMP) -S $U/initcode.o > $U/initcode.asm 

$U/pcalltemplate: $U/pcalltemplate.S
	$(CC) $(CFLAGS) -nostdinc -I. -Ikernel -c $U/pcalltemplate\.S -o $U/pcalltemplate.o
	$(LD) $(LDFLAGS) -N -e start -Ttext 0 -o $U/pcalltemplate\.out $U/pcalltemplate.o
	$(OBJCOPY) -S -O binary $U/pcalltemplate\.out $U/pcalltemplate
	$(OBJDUMP) -S $U/pcalltemplate\.o > $U/pcalltemplate.asm 

$U/_sqlitetest: $U/bla.c 
	$(CC)   -DSQLITE_TEMP_STORE=3  -DSQLITE_THREADSAFE=0 -DSQLITE_OS_OTHER=1  user/bla.c user/newlib_syscall.S -I. -lsodium -lc -lm -lgcc -o $U/_sqlitetest


$U/_ccalltest: $U/ccalltest.c 
	$(CC) -g  -DSQLITE_TEMP_STORE=3  -DSQLITE_THREADSAFE=0 -DSQLITE_OS_OTHER=1 user/newlib_syscall.S user/ccalltest.c user/modecall.S  user/usys.S -I. -o $U/_ccalltest -lsodium -lc -lm -lgcc
$U/_pipetest: $U/pipetest.c 
	$(CC) -g  -DSQLITE_TEMP_STORE=3  -DSQLITE_THREADSAFE=0 -DSQLITE_OS_OTHER=1 user/newlib_syscall.S user/pipetest.c user/usys.S -I. -o $U/_pipetest -lc -lm -lgcc
$U/_unsecuresql: $U/unsecure.c 
	$(CC) -g  -DSQLITE_TEMP_STORE=3  -DSQLITE_THREADSAFE=0 -DSQLITE_OS_OTHER=1  user/unsecure.c user/newlib_syscall.S user/test_demovfs.c user/sqlite3.c -I. -lsodium -lc -lm -lgcc -o $U/_unsecuresql
	riscv64-elf-objcopy --only-keep-debug $U/_unsecuresql $U/unsecure.debug
	$(STRIP) --strip-debug --strip-unneeded $U/_unsecuresql
$U/_procsecsql: $U/procsec.c 
	$(CC) -g  -DSQLITE_TEMP_STORE=3  -DSQLITE_THREADSAFE=0 -DSQLITE_OS_OTHER=1 user/usys.S user/procsec.c user/newlib_syscall.S user/test_demovfs.c user/sqlite3.c -I. -lsodium -lc -lm -lgcc -o $U/_procsecsql
	riscv64-elf-objcopy --only-keep-debug $U/_procsecsql $U/procsec.debug
	$(STRIP) --strip-debug --strip-unneeded $U/_procsecsql
$U/_securesql: $U/secure.c 
	$(CC) -g  -DSQLITE_TEMP_STORE=3  -DSQLITE_THREADSAFE=0 -DSQLITE_OS_OTHER=1  user/secure.c user/newlib_syscall.S user/test_demovfs.c user/usys.S user/modecall.S user/sqlite3.c -I. -lsodium -lc -lm -lgcc -o $U/_securesql
	$(OBJCOPY) --only-keep-debug $U/_securesql $U/secure.debug
	$(STRIP) --strip-debug --strip-unneeded $U/_securesql

tags: $(OBJS) _init
	etags *.S *.c

ULIB = $U/ulib.o $U/usys.o $U/printf.o $U/umalloc.o

_%: %.o $(ULIB)
	$(LD) $(LDFLAGS) -N -e main -Ttext 0 -o $@ $^
	$(OBJDUMP) -S $@ > $*.asm
	$(OBJDUMP) -t $@ | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $*.sym

$U/usys.S : $U/usys.pl
	perl $U/usys.pl > $U/usys.S

$U/usys.o : $U/usys.S
	$(CC) $(CFLAGS) -c -o $U/usys.o $U/usys.S

$U/_forktest: $U/forktest.o $(ULIB)
	# forktest has less library code linked in - needs to be small
	# in order to be able to max out the proc table.
	$(LD) $(LDFLAGS) -N -e main -Ttext 0 -o $U/_forktest $U/forktest.o $U/ulib.o $U/usys.o
	$(OBJDUMP) -S $U/_forktest > $U/forktest.asm

mkfs/mkfs: mkfs/mkfs.c $K/fs.h $K/param.h
	gcc -Werror -Wall -I. -o mkfs/mkfs mkfs/mkfs.c

# Prevent deletion of intermediate files, e.g. cat.o, after first build, so
# that disk image changes after first build are persistent until clean.  More
# details:
# http://www.gnu.org/software/make/manual/html_node/Chained-Rules.html
.PRECIOUS: %.o

UPROGS=\
	$U/_cat\
	$U/_date\
	$U/_echo\
	$U/_forktest\
	$U/_grep\
	$U/_hexdump\
	$U/_init\
	$U/_kill\
	$U/_ln\
	$U/_ls\
	$U/_mkdir\
	$U/_rm\
	$U/_sh\
	$U/_modetest\
	$U/_stressfs\
	$U/_usertests\
	$U/_grind\
	$U/_wc\
	$U/_zombie\
	$U/_shutdown\

unsecure.img: mkfs/mkfs README  $U/_pipetest  $U/_unsecuresql $U/_ccalltest user/test.db $U/pcalltemplate  $(UPROGS)
	mkfs/mkfs fs.img  $U/_pipetest $U/_unsecuresql  $U/_ccalltest user/test.db README $(UPROGS)

secure.img: mkfs/mkfs README  $U/_securesql $U/_ccalltest user/test.db  $(UPROGS)
	mkfs/mkfs fs.img  $U/_securesql  $U/_ccalltest user/test.db README $(UPROGS)
procsec.img: mkfs/mkfs README  $U/_procsecsql $U/_ccalltest user/test.db  $(UPROGS)
	mkfs/mkfs fs.img  $U/_procsecsql  $U/_ccalltest user/test.db README $(UPROGS)
-include kernel/*.d user/*.d

clean: 
	rm -f *.tex *.dvi *.idx *.aux *.log *.ind *.ilg \
	*/*.o */*.d */*.asm */*.sym \
	$U/initcode $U/initcode.out $K/kernel fs.img \
	mkfs/mkfs .gdbinit \
        $U/usys.S \
	$(UPROGS)
