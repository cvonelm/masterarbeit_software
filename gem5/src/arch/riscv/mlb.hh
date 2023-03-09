#ifndef __ARCH_RISCV_MLB_HH__
#define __ARCH_RISCV_MLB_HH__

#include "sim/sim_object.hh"
#include "arch/riscv/faults.hh"
#include "sim/faults.hh"
#include "params/MLB.hh"
#include <algorithm>
#include <cstdint>
#include <vector>
#include <array>
#include <list>
#include <string>

namespace gem5
{

#define MLB_FI 0x1
#define MLB_SI 0x2
class MLB : public SimObject
{
    public:
        PARAMS(MLB);
        MLB(const Params &params);
        
        int numEntries();
        
        Fault store_mode(uint64_t line, uint16_t cur_modeid, std::array<uint64_t, 3> data, uint32_t machInst);
        
        Fault load_mode(uint64_t line, uint16_t cur_modeid, std::array<uint64_t, 3> *data, uint32_t machInst);

        Fault get_parent(uint16_t modeid, uint16_t* parent);
        Fault get_entry_point(uint16_t modeid, uint64_t* entry_point);
        Fault get_epc(uint16_t modeid, uint64_t *epc);
        Fault set_epc(uint16_t modeid, uint64_t epc);
        
        Fault ignores_syscalls(uint16_t modeid, bool *ignore);
        Fault ignores_faults(uint16_t modeid, bool *ignore);
        struct __attribute__((packed)) mlb_entry
        {
            uint64_t entry_point;
            uint64_t epc;
            uint16_t modeid;
            uint16_t parent;
            uint32_t bits;
        };
    private:

        int get_line_for_modeid(uint16_t modeid);
        std::vector<struct mlb_entry> entries;
};
}
#endif
