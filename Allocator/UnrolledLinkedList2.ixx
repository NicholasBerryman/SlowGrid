//
// Created by nickberryman on 1/12/25.
//

module;
#include <type_traits>
#include "Logger.h"

export module SG_Allocator:UnrolledLinkedList2;
import :UnrolledLinkedList;
import :BaseArena;
import SG_AllocatorConfigs;
import Logger;

export namespace SG_Allocator {
    /**
     * @brief Homogenous unrolled doubly-linked list, with arena allocation support. Guarantees tightly packed allocations (unlike heterogenous version)
     *
     * @tparam InsideArenaType Arena type to allocate blocks into (Use a PseudoArena if heap allocation preferred)
     * @tparam alignment Data type to use for elements
     */
    template<typename InsideArenaType, typename alignment>
    requires std::is_base_of_v<BaseArena, InsideArenaType>
    class ULL2 {
    public:
        ULL2(InsideArenaType& arena, const arenaSize_t initialSize):
            impl(arena, initialSize){}
        
        inline void clear(){impl.clear();}
        inline void softClear(){impl.softClear();}
        inline void expand(const arenaSize_t& newSize){impl.expand(newSize);}
        
        inline const alignment& get(const arenaSize_t& index){return *impl.template get<alignment>(index); }
        inline void set(const arenaSize_t& index, const alignment& value){ (*impl.template get<alignment>(index)) = value; }
        inline void fill(const alignment& value){ for (arenaSize_t i = 0; i < impl.maxSize(); i++) (*impl.template get<alignment>(i)) = value; }
        
        inline void push_back(const alignment& value){ (*impl.template alloc<alignment>()) = value; }
        inline alignment pop_back(const alignment& value){ const alignment& out = (*impl.getFromBack(0)); impl.dealloc(1); return out;}
        inline const alignment& back(){ return (*impl.getFromBack(0)); }

        [[nodiscard]] inline arenaSize_t maxSize() const {return impl.maxSize();}
        [[nodiscard]] inline arenaSize_t length() const {return impl.length();}
    private:
        ULL<InsideArenaType, alignment> impl;
    };
}
