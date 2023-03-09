	#
        # interrupts and exceptions while in supervisor
        # mode come here.
        #
        # push all registers, call kerneltrap(), restore, return.
        #
.globl kerneltrap
.section trampsec
.globl trampoline
trampoline:
.align 4 
.globl pcallenter
pcallenter:

        csrrw a0, sscratch, a0
        sd a1, 72(a0)
        ld a1, 288(a0)

        beq a1,x0,fromuser
fromkernel:

        ld a1, 72(a0)
        csrrw a0, sscratch, a0
        addi sp, sp, -576

        # save the registers.
        sd ra, 0(sp)
        sd sp, 8(sp)
        sd gp, 16(sp)
        sd tp, 24(sp)
        sd t0, 32(sp)
        sd t1, 40(sp)
        sd t2, 48(sp)
        sd s0, 56(sp)
        sd s1, 64(sp)
        sd a0, 72(sp)
        sd a1, 80(sp)
        sd a2, 88(sp)
        sd a3, 96(sp)
        sd a4, 104(sp)
        sd a5, 112(sp)
        sd a6, 120(sp)
        sd a7, 128(sp)

        sd s2, 136(sp)
        sd s3, 144(sp)
        sd s4, 152(sp)
        sd s5, 160(sp)
        sd s6, 168(sp)
        sd s7, 176(sp)
        sd s8, 184(sp)
        sd s9, 192(sp)
        sd s10, 200(sp)
        sd s11, 208(sp)
        sd t3, 216(sp)
        sd t4, 224(sp)
        sd t5, 232(sp)
        sd t6, 240(sp)
fsd f0, 296(sp)
fsd f1, 304(sp)
fsd f2, 312(sp)
fsd f3, 320(sp)
fsd f4, 328(sp)
fsd f5, 336(sp)
fsd f6, 344(sp)
fsd f7, 352(sp)
fsd f8, 360(sp)
fsd f9, 368(sp)
fsd f10, 376(sp)
fsd f11, 384(sp)
fsd f12, 392(sp)
fsd f13, 400(sp)
fsd f14, 408(sp)
fsd f15, 416(sp)
fsd f16, 424(sp)
fsd f17, 432(sp)
fsd f18, 440(sp)
fsd f19, 448(sp)
fsd f20, 456(sp)
fsd f21, 464(sp)
fsd f22, 472(sp)
fsd f23, 480(sp)
fsd f24, 488(sp)
fsd f25, 496(sp)
fsd f26, 504(sp)
fsd f27, 512(sp)
fsd f28, 520(sp)
fsd f29, 528(sp)
fsd f30, 536(sp)
fsd f31, 544(sp)
csrr a0, fcsr
sd a0, 552(sp)
csrr a0, frm
sd a0, 560(sp)
csrr a0, fflags
sd a0, 568(sp)
        csrr a0, sepc
        sd a0, 248(sp)
        
        csrr a0, sscratch
        ld t0, 16(a0)
        sd sp, 8(a0)
        jr t0
fromuser:
ld a1, 72(a0)
         sd ra, 40(a0)
         sd sp, 48(a0)
         sd gp, 56(a0)
         sd tp, 64(a0)
         sd t0, 72(a0)
         sd t1, 80(a0)
         sd t2, 88(a0)
         sd s0, 96(a0)
         sd s1, 104(a0)
         sd a1, 120(a0)
         sd a2, 128(a0)
         sd a3, 136(a0) 
         sd a4, 144(a0)
         sd a5, 152(a0)
         sd a6, 160(a0)
         sd a7, 168(a0)
         sd s2, 176(a0)
         sd s3, 184(a0)
         sd s4, 192(a0)
         sd s5, 200(a0)
         sd s6, 208(a0)
         sd s7, 216(a0)
         sd s8, 224(a0)
         sd s9, 232(a0)
         sd s10, 240(a0)
         sd s11, 248(a0)
         sd t3, 256(a0)
         sd t4, 264(a0)
         sd t5, 272(a0)
         sd t6, 280(a0)
        fsd f0, 296(a0)
fsd f1, 304(a0)
fsd f2, 312(a0)
fsd f3, 320(a0)
fsd f4, 328(a0)
fsd f5, 336(a0)
fsd f6, 344(a0)
fsd f7, 352(a0)
fsd f8, 360(a0)
fsd f9, 368(a0)
fsd f10, 376(a0)
fsd f11, 384(a0)
fsd f12, 392(a0)
fsd f13, 400(a0)
fsd f14, 408(a0)
fsd f15, 416(a0)
fsd f16, 424(a0)
fsd f17, 432(a0)
fsd f18, 440(a0)
fsd f19, 448(a0)
fsd f20, 456(a0)
fsd f21, 464(a0)
fsd f22, 472(a0)
fsd f23, 480(a0)
fsd f24, 488(a0)
fsd f25, 496(a0)
fsd f26, 504(a0)
fsd f27, 512(a0)
fsd f28, 520(a0)
fsd f29, 528(a0)
fsd f30, 536(a0)
fsd f31, 544(a0)
csrr a4, fcsr
sd a4, 552(a0)
csrr a4, frm
sd a4, 560(a0)
csrr a4, fflags
sd a4, 568(a0)
        // save the registers.
         # save the user a0 in p->trapframe->a0
         csrr t0, sscratch
         sd t0, 112(a0)

         # restore kernel stack pointer from p->trapframe->kernel_sp
         ld sp, 8(a0)
 
         # make tp hold the current hartid, from p->trapframe->kernel_hartid
         ld tp, 32(a0)
 
         # load the address of usertrap(), p->trapframe->kernel_trap
         ld t0, 16(a0)
 
         # restore kernel page table from p->trapframe->kernel_satp
         ld t1, 0(a0)
         csrw satp, t1
         sfence.vma zero, zero
 
         # a0 is no longer valid, since the kernel page
         # table does not specially map p->tf.
 
         # jump to usertrap(), which does not return
         jr t0

.globl kerneltrapret
kerneltrapret:
ld a4, 552(sp)
csrw fcsr, a4
ld a4, 560(sp)
csrw frm, a4
ld a4, 568(sp)
csrw fflags, a4
        add sp, a1, x0
        ld a2, 248(sp)
        sd a2, 8(a0)
        mlbstore x0,a0
        ld ra, 0(sp)
        ld gp, 16(sp)
        // not this, in case we moved CPUs: ld tp, 24(sp)
        ld t0, 32(sp)
        ld t1, 40(sp)
        ld t2, 48(sp)
        ld s0, 56(sp)
        ld s1, 64(sp)
        ld a0, 72(sp)
        ld a1, 80(sp)
        ld a2, 88(sp)
        ld a3, 96(sp)
        ld a4, 104(sp)
        ld a5, 112(sp)
        ld a6, 120(sp)
        ld a7, 128(sp)
        ld s2, 136(sp)
        ld s3, 144(sp)
        ld s4, 152(sp)
        ld s5, 160(sp)
        ld s6, 168(sp)
        ld s7, 176(sp)
        ld s8, 184(sp)
        ld s9, 192(sp)
        ld s10, 200(sp)
        ld s11, 208(sp)
        ld t3, 216(sp)
        ld t4, 224(sp)
        ld t5, 232(sp)
        ld t6, 240(sp)
fld f0, 296(sp)
fld f1, 304(sp)
fld f2, 312(sp)
fld f3, 320(sp)
fld f4, 328(sp)
fld f5, 336(sp)
fld f6, 344(sp)
fld f7, 352(sp)
fld f8, 360(sp)
fld f9, 368(sp)
fld f10, 376(sp)
fld f11, 384(sp)
fld f12, 392(sp)
fld f13, 400(sp)
fld f14, 408(sp)
fld f15, 416(sp)
fld f16, 424(sp)
fld f17, 432(sp)
fld f18, 440(sp)
fld f19, 448(sp)
fld f20, 456(sp)
fld f21, 464(sp)
fld f22, 472(sp)
fld f23, 480(sp)
fld f24, 488(sp)
fld f25, 496(sp)
fld f26, 504(sp)
fld f27, 512(sp)
fld f28, 520(sp)
fld f29, 528(sp)
fld f30, 536(sp)
fld f31, 544(sp)
        addi sp, sp, 576

        preturn
.globl pcallret
pcallret:
        
                 # a0: TRAPFRAME, in user page table.
         # a1: user page table, for satp.
 
         # switch to the user page table.
         csrw satp, a1
         sfence.vma zero, zero
ld a4, 552(a0)
csrw fcsr, a4
ld a4, 560(a0)
csrw frm, a4
ld a4, 568(a0)
csrw fflags, a4
 
         # put the saved user a0 in sscratch, so we
         # can swap it with our a0 (TRAPFRAME) in the last step.
         ld t0, 112(a0)

         csrw sscratch, t0
         # restore all but a0 from TRAPFRAME
         ld ra, 40(a0)
         ld sp, 48(a0)
         ld gp, 56(a0)
         ld tp, 64(a0)
         ld t0, 72(a0)
         ld t1, 80(a0)
         ld t2, 88(a0)
         ld s0, 96(a0)
         ld s1, 104(a0)
         ld a1, 120(a0)
         ld a2, 128(a0)
         ld a3, 136(a0)
         ld a4, 144(a0)
         ld a5, 152(a0)
         ld a6, 160(a0)
         ld a7, 168(a0)
         ld s2, 176(a0)
         ld s3, 184(a0)
         ld s4, 192(a0)
         ld s5, 200(a0)
         ld s6, 208(a0)
         ld s7, 216(a0)
         ld s8, 224(a0)
         ld s9, 232(a0)
         ld s10, 240(a0)
         ld s11, 248(a0)
         ld t3, 256(a0)
         ld t4, 264(a0)
         ld t5, 272(a0)
         ld t6, 280(a0)
fld f0, 296(a0)
fld f1, 304(a0)
fld f2, 312(a0)
fld f3, 320(a0)
fld f4, 328(a0)
fld f5, 336(a0)
fld f6, 344(a0)
fld f7, 352(a0)
fld f8, 360(a0)
fld f9, 368(a0)
fld f10, 376(a0)
fld f11, 384(a0)
fld f12, 392(a0)
fld f13, 400(a0)
fld f14, 408(a0)
fld f15, 416(a0)
fld f16, 424(a0)
fld f17, 432(a0)
fld f18, 440(a0)
fld f19, 448(a0)
fld f20, 456(a0)
fld f21, 464(a0)
fld f22, 472(a0)
fld f23, 480(a0)
fld f24, 488(a0)
fld f25, 496(a0)
fld f26, 504(a0)
fld f27, 512(a0)
fld f28, 520(a0)
fld f29, 528(a0)
fld f30, 536(a0)
fld f31, 544(a0) 
         # restore user a0, and save TRAPFRAME in sscratch
         csrrw a0, sscratch, a0
 
         # return to user mode and user pc.
         # usertrapret() set up sstatus and sepc.
         preturn

.globl ccallfork

ccallfork: 
         # a0: TRAPFRAME, in user page table.
         # a1: user page table, for satp.
 
         # switch to the user page table.
         csrw satp, a1
         sfence.vma zero, zero
 
         # put the saved user a0 in sscratch, so we
         # can swap it with our a0 (TRAPFRAME) in the last step.
         ld t0, 112(a0)

         csrw sscratch, t0

         ld ra, 40(a0)
         ld sp, 48(a0)
         ld gp, 56(a0)
         ld tp, 64(a0)
         ld t0, 72(a0)
         ld t1, 80(a0)
         ld t2, 88(a0)
         ld s0, 96(a0)
         ld s1, 104(a0)
         ld a1, 120(a0)
         ld a2, 128(a0)
         ld a3, 136(a0)
         ld a4, 144(a0)
         ld a5, 152(a0)
         ld a6, 160(a0)
         ld a7, 168(a0)
         ld s2, 176(a0)
         ld s3, 184(a0)
         ld s4, 192(a0)
         ld s5, 200(a0)
         ld s6, 208(a0)
         ld s7, 216(a0)
         ld s8, 224(a0)
         ld s9, 232(a0)
         ld s10, 240(a0)
         ld s11, 248(a0)
         ld t3, 256(a0)
         ld t4, 264(a0)
         ld t5, 272(a0)
         ld t6, 280(a0)
 
         # restore user a0, and save TRAPFRAME in sscratch
         csrrw a0, sscratch, a0
 
         ccall
