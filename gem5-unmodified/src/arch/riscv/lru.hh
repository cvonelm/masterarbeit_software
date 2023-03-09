#ifndef __ARCH_RISCV_LRU_HH__
#define __ARCH_RISCV_LRU_HH__

#include "sim/sim_object.hh"
#include <list>

class LRU
 {
 public:
     void print_lru()
           {
               std::string ret;
               for(auto i : lru)
               {
                   ret.append(std::to_string(i) + ":");
               }
               warn(ret);
           }
 
         LRU(size_t size)
         {
             for(int i = 1; i < size; i++)
             {
                 lru.push_back(i);
             }
         }
 
         size_t get_lru()
         {
             size_t elem = lru.front();
             lru.pop_front();
             lru.push_back(elem);
             return elem;
         }
 
         void access(size_t elem)
         {
             if(elem == 0)
             {
                 return;
             }
             auto it = std::find(lru.begin(), lru.end(), elem);
             lru.erase(it);
             lru.push_back(elem);
         }
     private:
         std::list<size_t> lru;
 
 };
 
#endif
