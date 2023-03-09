#include "arch/riscv/faults.hh"
#include "arch/riscv/plb.hh"
#include "arch/riscv/pagetable.hh"
#include "arch/riscv/faults.hh"
#include "arch/riscv/regs/misc.hh"
#include <cstring>
#include <cstdint>

namespace gem5
{
 void plb_set_size_perm(struct plb_entry *entry, uint64_t perm, uint64_t size)
 {
     perm = perm << 45;
     size = size | perm;
     memcpy(entry->size_perm, &size, 6);
 }
 
 uint64_t plb_get_perm(struct plb_entry *entry)
 {
     uint64_t perm = 0;
     memcpy(&perm, entry->size_perm, 6);
     return perm >> 45;
 }
 
 uint64_t plb_get_size(struct plb_entry *entry)
 {
     uint64_t size = 0;
     memcpy(&size, entry->size_perm, 6);
     uint64_t perm_mask = ~(0x7ULL << 45);
     size = size & perm_mask;
     return size;
 }

PLB::PLB(const Params &params)
            : SimObject(params),
            entries(params.size), size(params.size)
        {

            for(auto &entry : entries)
            {
                entry.modeid = 0;
            }
        }
        
        Fault PLB::store_line(uint64_t line, std::array<uint64_t, 2> data, uint32_t machInst)
        {
            struct plb_entry *bla = (struct plb_entry*)data.data();
            warn("stored line for mode %d, start  %llx, end %llx, perm %d", bla->modeid, bla->start, plb_get_size(bla), plb_get_perm(bla));
            entries.at(line) = *(struct plb_entry*)data.data();
            return NoFault;
        }

        Fault PLB::load_line(uint64_t line, std::array<uint64_t, 2> *data, uint32_t machInst)
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
                    uint64_t perm = plb_get_perm(&entry);
                    if(vaddr >= entry.start && vaddr < entry.start + plb_get_size(&entry))
                    {
                        if(mode== BaseMMU::Read && !(perm & PLB_R))
                        {
                    warn("Load failed!!");
                            return std::make_shared<RiscvISA::AddressFault>(req->getVaddr(), RiscvISA::ExceptionCode::LOAD_ACCESS);
                        }
                        else if(mode == BaseMMU::Write && !(perm & PLB_W))
                        {
                    warn("Load failed!!");
                            return std::make_shared<RiscvISA::AddressFault>(req->getVaddr(), RiscvISA::ExceptionCode::STORE_ACCESS);
                        }
                        else if(!(perm & PLB_X))
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
