/*
 * Copyright (c) 2016 RISC-V Foundation
 * Copyright (c) 2016 The University of Virginia
 * Copyright (c) 2018 TU Dresden
 * Copyright (c) 2020 Barkhausen Institut
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "arch/riscv/faults.hh"

#include "arch/riscv/insts/static_inst.hh"
#include "arch/riscv/isa.hh"
#include "arch/riscv/regs/misc.hh"
#include "arch/riscv/utility.hh"
#include "cpu/base.hh"
#include "cpu/thread_context.hh"
#include "debug/Faults.hh"
#include "sim/debug.hh"
#include "sim/full_system.hh"
#include "sim/workload.hh"

namespace gem5
{

namespace RiscvISA
{

void RiscvFault::invokeSE(ThreadContext *tc, const StaticInstPtr &inst)
{
    panic("Fault %s encountered at pc %s.", name(), tc->pcState());
}

void RiscvFault::invoke(ThreadContext *tc, const StaticInstPtr &inst)
{
    invoke_inner(tc, inst, tc->pcState().instAddr(), tc->readMiscReg(MISCREG_MODEID));
}

void RiscvFault::invoke_inner(ThreadContext *tc, const StaticInstPtr &inst, Addr exception_addr, uint64_t exception_modeid)
{
    auto pc_state = tc->pcState().as<PCState>();

    DPRINTFS(Faults, tc->getCpuPtr(), "Fault (%s) at PC: %s\n",
             name(), pc_state);

    if (FullSystem) {
        PrivilegeMode pp = (PrivilegeMode)tc->readMiscReg(MISCREG_PRV);
        PrivilegeMode prv = PRV_M;
        STATUS status = tc->readMiscReg(MISCREG_STATUS);

        if (isInterrupt()) {
            if (pp != PRV_M &&
                bits(tc->readMiscReg(MISCREG_MIDELEG), _code) != 0) {
                prv = PRV_S;
            }
        } else {
            if (pp != PRV_M &&
                bits(tc->readMiscReg(MISCREG_MEDELEG), _code) != 0) {
                prv = PRV_S;
            }
        }

        
        Addr addr;

        // Set fault registers and status
        MiscRegIndex  cause, tval;
        ISA *isa;
        Fault mlb_fault;
        uint16_t parent = 0;
        switch (prv) {
          case PRV_S:
            
            if(_code == ECALL_SUPER && !isInterrupt())
            {
                exception_addr += 4;
            }
            
            tc->setMiscReg(MISCREG_SEPC, exception_addr);
            tc->setMiscReg(MISCREG_TARGETMODE, tc->readMiscReg(MISCREG_MODEID));
            cause = MISCREG_SCAUSE;
            tval = MISCREG_STVAL;
            status.spie = status.sie;
            status.sie = 0;
            isa = static_cast<ISA*>(tc->getIsaPtr());
            
            mlb_fault = isa->getMLB()->set_epc(tc->readMiscReg(MISCREG_MODEID), exception_addr);
            if(mlb_fault != NoFault)
            {
                warn("Could not set EPC for mode %d\n", tc->readMiscReg(MISCREG_MODEID));
                while(1);
            }
            if(mlb_fault != NoFault)
            {
                 isa->set_saved_fault(std::make_shared<RiscvFault>(*this), exception_addr, exception_modeid);
                 dynamic_cast<RiscvFault*>(mlb_fault.get())->invoke(tc, inst);
                 return;
            }
            if(isInterrupt())
            {
                parent = 1;
            }
            else
            {
                while(true)
                {
                    mlb_fault = isa->getMLB()->get_parent(exception_modeid, &parent);
                    if(mlb_fault != NoFault)
                    {
                        isa->set_saved_fault(std::make_shared<RiscvFault>(*this), exception_addr, exception_modeid);
                        dynamic_cast<RiscvFault*>(mlb_fault.get())->invoke(tc, inst);
                        return;
                    }

                    if(parent == 1)
                    {
                        break;
                    }
                    if(_code == ECALL_SUPER)
                    {
                        //If the sys bit is set, we have a parent call, those cant be ignored.
                        uint64_t sys_bit = 1ULL << 63;
                        if(tc->readMiscReg(MISCREG_TARGET) && sys_bit)
                        {
                            break;
                        }
                        bool ignore;
                        mlb_fault = isa->getMLB()->ignores_syscalls(parent, &ignore);
                        if(mlb_fault != NoFault)
                        {
                            isa->set_saved_fault(std::make_shared<RiscvFault>(*this), exception_addr, exception_modeid);
                            dynamic_cast<RiscvFault*>(mlb_fault.get())->invoke(tc, inst);
                            return;
                        }
                        if(!ignore)
                        {
                            break;
                        }
                    }
                    else
                    {
                        bool ignore;
                        mlb_fault = isa->getMLB()->ignores_faults(parent, &ignore);
                        if(mlb_fault != NoFault)
                        {
                            isa->set_saved_fault(std::make_shared<RiscvFault>(*this), exception_addr, exception_modeid);
                            dynamic_cast<RiscvFault*>(mlb_fault.get())->invoke(tc, inst);
                            return;
                        }
                        if(!ignore)
                        {
                            break;
                        }

                    }

                }


            }    warn("Parent is: %d", parent);
                
                mlb_fault = isa->getMLB()->get_entry_point(parent, &addr);
                warn("Entry point is: %d\n", addr);
                if(mlb_fault != NoFault)
                {
                    isa->set_saved_fault(std::make_shared<RiscvFault>(*this), exception_addr, exception_modeid);
                    dynamic_cast<RiscvFault*>(mlb_fault.get())->invoke(tc, inst);
                    return;
                }
            
            break;
          case PRV_M:
            tc->setMiscReg(MISCREG_MEPC, exception_addr);
            cause = MISCREG_MCAUSE;
            tval = MISCREG_MTVAL;
            addr = mbits(tc->readMiscReg(MISCREG_MTVEC), 63, 2);
            status.mpp = pp;
            status.mpie = status.mie;
            status.mie = 0;
            break;
          default:
            panic("Unknown privilege mode %d.", prv);
            break;
        }

        // Interrupt is indicated on the MSB of cause (bit 63 in RV64)
        uint64_t _cause = _code;
        if (isInterrupt()) {
           _cause |= (1L << 63);
        }

        tc->setMiscReg(MISCREG_MODEID, parent);
        tc->setMiscReg(cause, _cause);
        tc->setMiscReg(tval, trap_value());
        tc->setMiscReg(MISCREG_PRV, prv);
        tc->setMiscReg(MISCREG_STATUS, status);
        // Temporarily mask NMI while we're in NMI handler. Otherweise, the
        // checkNonMaskableInterrupt will always return true and we'll be
        // stucked in an infinite loop.
        if (isNonMaskableInterrupt()) {
            tc->setMiscReg(MISCREG_NMIE, 0);
        }
        pc_state.set(addr);
        tc->pcState(pc_state);
    } else {
        inst->advancePC(pc_state);
        tc->pcState(pc_state);
        invokeSE(tc, inst);
    }
}

void
Reset::invoke(ThreadContext *tc, const StaticInstPtr &inst)
{
    tc->setMiscReg(MISCREG_PRV, PRV_M);
    STATUS status = tc->readMiscReg(MISCREG_STATUS);
    status.mie = 0;
    status.mprv = 0;
    tc->setMiscReg(MISCREG_STATUS, status);
    tc->setMiscReg(MISCREG_MCAUSE, 0);

    // Advance the PC to the implementation-defined reset vector
    auto workload = dynamic_cast<Workload *>(tc->getSystemPtr()->workload);
    PCState pc(workload->getEntry());
    tc->pcState(pc);
}

void
UnknownInstFault::invokeSE(ThreadContext *tc, const StaticInstPtr &inst)
{
    auto *rsi = static_cast<RiscvStaticInst *>(inst.get());
    panic("Unknown instruction 0x%08x at pc %s", rsi->machInst,
        tc->pcState());
}

void
IllegalInstFault::invokeSE(ThreadContext *tc, const StaticInstPtr &inst)
{
    auto *rsi = static_cast<RiscvStaticInst *>(inst.get());
    panic("Illegal instruction 0x%08x at pc %s: %s", rsi->machInst,
        tc->pcState(), reason.c_str());
}

void
UnimplementedFault::invokeSE(ThreadContext *tc, const StaticInstPtr &inst)
{
    panic("Unimplemented instruction %s at pc %s", instName, tc->pcState());
}

void
IllegalFrmFault::invokeSE(ThreadContext *tc, const StaticInstPtr &inst)
{
    panic("Illegal floating-point rounding mode 0x%x at pc %s.",
            frm, tc->pcState());
}

void
BreakpointFault::invokeSE(ThreadContext *tc, const StaticInstPtr &inst)
{
    schedRelBreak(0);
}

void
SyscallFault::invokeSE(ThreadContext *tc, const StaticInstPtr &inst)
{
    tc->getSystemPtr()->workload->syscall(tc);
}

} // namespace RiscvISA
} // namespace gem5
