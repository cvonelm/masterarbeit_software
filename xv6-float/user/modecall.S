.globl scall
scall:
        pcall
        ret
.globl parentcall
parentcall:
        li t3, 1
        slli t3, t3, 63
        or a1, a1, t3
        csrw ccalladdr, a1
        ld t0, 128(a0)
        addi t0, t0, -16
        sd t0, 128(a0)
        csrr t3, targetmode
        sd t3, 0(t0)
        csrr t4, modeid
        mepcload t3,t4
        sd t3, 8(t0)
        addi sp, sp, -8
        sd a0, 0(sp)
        mv a0, a2
        pcall
        ld t0, 0(sp)
        addi sp, sp, 8
        ld t1, 128(t0)
        ld t3, 0(t1)
        csrw targetmode, t3
        csrr t4, modeid
        ld t3, 8(t1)
        mepcstore t3, t4
        addi t1, t1, 16
        sd t1, 128(t0)
        ret

.globl childcall
childcall:
        addi sp, sp, -16
        sd ra, 8(sp)
        sd s0, 0(sp)
        addi s0, sp, 16
        csrw targetmode, a1
        csrw ccalladdr, a2
        ld t0, 128(a0)
        addi t0, t0, -16
        sd t0, 128(a0)
        csrr t3, targetmode
        sd t3, 0(t0)
        csrr t4, modeid
        mepcload t3,t4
        sd t3, 8(t0)
        addi sp, sp, -8
        sd a0, 0(sp)
        mv a0, a3
        la ra, childret
        ccall
        ld t0, 0(sp)
        addi sp, sp, 8
        ld t1, 128(t0)
        ld t3, 0(t1)
        csrw targetmode, t3
        csrr t4, modeid
        ld t3, 8(t1)
        mepcstore t3, t4
        addi t1, t1, 16
        sd t1, 128(t0)
        ld ra, 8(sp)
        ld s0, 0(sp)
        addi sp, sp, 16
        ret

childret:
        creturn
