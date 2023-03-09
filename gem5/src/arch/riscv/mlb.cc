#include "arch/riscv/faults.hh"
#include "arch/riscv/mlb.hh"

#include <cstring>

#define MLB_SI 0x2
#define MLB_FI 0x1

namespace gem5
{

MLB::MLB(const Params &params)
            : SimObject(params),
            entries(params.size)
        {
        }
        
        Fault MLB::store_mode(uint64_t line, uint16_t cur_modeid, std::array<uint64_t, 3> data, uint32_t machInst)
        {

            if(cur_modeid != 1)
            {
                return std::make_shared<RiscvISA::IllegalInstFault>("mode!=1 trying to change other mode", machInst);
            }

            struct mlb_entry *entry = (struct mlb_entry*)(data.data());
            entries.at(line) = *entry;
            return NoFault;
        }

        Fault MLB::load_mode(uint64_t line, uint16_t cur_modeid, std::array<uint64_t, 3> *data, uint32_t machInst)
        {
            if(cur_modeid != 1)
            {
                return std::make_shared<RiscvISA::IllegalInstFault>("mode!=1 trying to load other mode", machInst);
            }
            
            memcpy(data->data(), &entries.at(line), sizeof(mlb_entry));

            return NoFault;
        }

        Fault MLB::get_parent(uint16_t modeid, uint16_t* parent)
        {
            int line = get_line_for_modeid(modeid);

            if(line == -1)
            {
                warn("Couldn't get parent for: %d", modeid);
                return std::make_shared<RiscvISA::MlbFault>(modeid);
            }

            *parent = entries.at(line).parent;
            return NoFault;
        }
        
        Fault MLB::get_entry_point(uint16_t modeid, uint64_t* entry_point)
        {
            int line = get_line_for_modeid(modeid);
            if(line == -1)
            {
                warn("Couldn't get ep for: %d", modeid);
                return std::make_shared<RiscvISA::MlbFault>(modeid);
            }

            *entry_point = entries.at(line).entry_point;
            return NoFault;
        }

        Fault MLB::get_epc(uint16_t modeid, uint64_t *epc)
        {
            int line = get_line_for_modeid(modeid);

            if(line == -1)
            {

                warn("Couldn't get epc for: %d", modeid);
                return std::make_shared<RiscvISA::MlbFault>(modeid);
            }

            *epc = entries.at(line).epc;

            return NoFault;
        }

        Fault MLB::set_epc(uint16_t modeid, uint64_t epc)
        {
            int line = get_line_for_modeid(modeid);

            if(line == -1)
            {
                warn("Couldn't get epc for: %d", modeid);
                return std::make_shared<RiscvISA::MlbFault>(modeid);
            }

            entries.at(line).epc = epc;

            return NoFault;
        }

        Fault MLB::ignores_syscalls(uint16_t modeid, bool *ignore)
        {
            int line = get_line_for_modeid(modeid);

            if(line == -1)
            {
                warn("Couldn't get epc for: %d", modeid);
                return std::make_shared<RiscvISA::MlbFault>(modeid);
            }
            *ignore =  entries.at(line).bits && MLB_SI;

            return NoFault;
        }
        
        Fault MLB::ignores_faults(uint16_t modeid, bool* ignore)
        {
            int line = get_line_for_modeid(modeid);

            if(line == -1)
            {
                warn("Couldn't get epc for: %d", modeid);
                return std::make_shared<RiscvISA::MlbFault>(modeid);
            }
            *ignore = entries.at(line).bits && MLB_FI;

            return NoFault;

        }

        int MLB::get_line_for_modeid(uint16_t modeid)
        {
            for(int i = 0; i < entries.size(); i++)
            {
                if(entries.at(i).modeid == modeid)
                {
                    return i;
                }
            }
            return -1;
        }
}
