#ifndef __ARCH_RISCV_PLB_HH__
#define __ARCH_RISCV_PLB_HH__

#include "sim/sim_object.hh"
#include "arch/riscv/faults.hh"
#include "sim/faults.hh"
#include "params/PLB.hh"
#include <algorithm>
#include <cstdint>
#include <vector>
#include <array>
#include <list>
#include <string>
#include "arch/generic/mmu.hh"
namespace gem5
{
enum bits {
    PLB_R = 0x1,
    PLB_W = 0x2,
    PLB_X = 0x4

};
        struct __attribute__((packed)) plb_entry
        {
            uint16_t modeid;
            uint64_t start;
            char size_perm[6];
        };

class PLB : public SimObject
{
    public:
        PARAMS(PLB);
        PLB(const Params &params);
        
        int numEntries();
        
        Fault store_line( uint64_t line, std::array<uint64_t, 2> data, uint32_t machInst);
        
        Fault load_line(uint64_t line, std::array<uint64_t, 2> *data, uint32_t machInst);
        
        Fault plbCheck(const RequestPtr &req, BaseMMU::Mode mode, ThreadContext *tc);
    private:

        std::vector<struct plb_entry> entries;
        size_t size;
};
}
#endif
