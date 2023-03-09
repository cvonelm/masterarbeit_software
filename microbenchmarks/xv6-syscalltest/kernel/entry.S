	# qemu -kernel loads the kernel at 0x80000000
        # and causes each CPU to jump there.
        # kernel.ld causes the following code to
        # be placed at 0x80000000.
.section .text
.global _entry
_entry:
        # put the hartid into a0
        csrr a0, mhartid

        # zero out the bss section
        # only the core with hartid = 0 does it
        bnez a0, bss_wait
        la t1, __bss_start
        la t2, __bss_end
        ble t1, t2, clear_bss_done
clear_bss:
        sd zero, 0(t1)
        addi t1, t1, 8
        blt t1, t2, clear_bss
clear_bss_done:
        fence
        sw zero, bss_nflag, t3
        j bss_wait_done
bss_wait:
        # cores with hartid != 0 wait for until the bss is cleared
        lw t4, bss_nflag
        bnez t4, bss_wait
        fence
bss_wait_done:

	# set up a stack for C.
        # stack0 is declared in start.c,
        # with a 4096-byte stack per CPU.
        # sp = stack0 + (hartid * 4096)
        la sp, stack0
        li t1, 1024*4
        addi t2, a0, 1
        mul t1, t1, t2
        add sp, sp, t1

	# jump to start() in start.c
        call start
spin:
        j spin
