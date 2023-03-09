.global ccalltest
ccalltest:
addi    sp,sp,-16
sd      s0,8(sp)
addi    s0,sp,16
ld      s0,8(sp)
addi    sp,sp,16
creturn

