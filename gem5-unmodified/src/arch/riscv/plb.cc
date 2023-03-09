#include "arch/riscv/faults.hh"
#include "arch/riscv/plb.hh"
#include "arch/riscv/pagetable.hh"
#include "arch/riscv/faults.hh"
#include "arch/riscv/regs/misc.hh"
#include <cstring>
namespace gem5
{

PLB::PLB(const Params &params)
            : SimObject(params),
            entries(params.size), size(params.size)
        {

            for(auto &entry : entries)
            {
                entry.modeid = 0;
            }
        }
        
        Fault PLB::store_line(uint64_t line, std::array<uint64_t, 3> data, uint32_t machInst)
        {
            struct plb_entry *bla = (struct plb_entry*)data.data();
            warn("stored line for mode %d, start %d, end %d, perm %d", bla->modeid, bla->start, bla->end, bla->bits);
            entries.at(line) = *(struct plb_entry*)data.data();
            return NoFault;
        }

        Fault PLB::load_line(uint64_t line, std::array<uint64_t, 3> *data, uint32_t machInst)
        {
            memcpy(data->data(), &entries.at(line), sizeof(plb_entry));
            return NoFault;
        }

        Fault PLB::plbCheck(const RequestPtr &req, BaseMMU::Mode mode, ThreadContext *tc)
        {
            if(!req->hasVaddr())
            {
                return NoFault;
            }
            if(tc->readMiscReg(RiscvISA::MISCREG_MODEID) > 1)
            {
                warn("IN CHILD");
            }
            Addr vaddr = Addr(sext<gem5::RiscvISA::VADDR_BITS>(req->getVaddr()));
            if(tc->readMiscReg(RiscvISA::MISCREG_MODEID) <= 1)
            {
                return NoFault;
            }
            int i = 0;
            for(; i < size; i++)
            {
                auto entry = entries.at(i);
                if(entry.modeid == tc->readMiscReg(RiscvISA::MISCREG_MODEID))
                {
                    warn("Found Entry!");
                    if(vaddr > entry.start && vaddr < entry.start + entry.end)
                    {
                        if(mode== BaseMMU::Read && !(entry.bits & PLB_R))
                        {
                    warn("Load failed!!");
                            return std::make_shared<RiscvISA::AddressFault>(req->getVaddr(), RiscvISA::ExceptionCode::LOAD_ACCESS);
                        }
                        else if(mode == BaseMMU::Write && !(entry.bits & PLB_W))
                        {
                    warn("Load failed!!");
                            return std::make_shared<RiscvISA::AddressFault>(req->getVaddr(), RiscvISA::ExceptionCode::STORE_ACCESS);
                        }
                        else if(!(entry.bits & PLB_X))
                        {
                    warn("Load failed!!");
                            return std::make_shared<RiscvISA::AddressFault>(req->getVaddr(), RiscvISA::ExceptionCode::INST_ACCESS);
                        }
                        return NoFault;
                    }
                }
            }
                        if(mode == BaseMMU::Read)
                        {
                    warn("Load failed!!");
                            return std::make_shared<RiscvISA::AddressFault>(req->getVaddr(), RiscvISA::ExceptionCode::LOAD_ACCESS);
                        }
                        else if(mode == BaseMMU::Write)
                        {
                    warn("Load failed!!");
                            return std::make_shared<RiscvISA::AddressFault>(req->getVaddr(), RiscvISA::ExceptionCode::STORE_ACCESS);
                        }
                        else 
                        {
                    warn("Load failed!!");
                            return std::make_shared<RiscvISA::AddressFault>(req->getVaddr(), RiscvISA::ExceptionCode::INST_ACCESS);
                        }
        }
}
