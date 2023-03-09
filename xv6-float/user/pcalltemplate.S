.globl pcalltemplate
pcalltemplate:
        la t0, end
        ld t1, 0(t0) // now holds the address of the mode local stack
        addi t1, t1, -16
        sd t1, 0(t0)
        csrr t3, targetmode
        sd t3, 0(t1)
        csrr t4, modeid
        mepcload t3, t4
        sd t3, 8(t1)
        addi sp, sp, -16
        sd ra, 8(sp)
        sd s0, 0(sp)
        csrr t3, ccalladdr 
        srli t3, t3, 63
        beqz t3, sysc
        ld t3, 8(t0)
        jalr ra, t3, 0
        j return
sysc:
        ld t3, 16(t0)
        jalr ra, t3, 0
        j return
return:
        la t0, end
        ld t1, 0(t0)
        ld t3, 0(t1)
        csrw targetmode, t3
        csrr t4, modeid
        ld t3, 8(t1)
        mepcstore t3, t4
        addi t1, t1, 16
        sd t1, 0(t0)
        ld ra, 8(sp)
        ld s0, 0(sp)
        addi sp, sp, 16
        preturn
        nop
        nop
end:
