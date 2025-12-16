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
     * @tparam T Data type to use for elements
     */
    template<typename InsideArenaType, typename T>
    requires std::is_base_of_v<BaseArena, InsideArenaType>
    class ULL2 {
    public:
        ULL2(InsideArenaType& arena, const arenaSize_t initialSize):
            impl(arena, initialSize){}
        
        inline void clear(){impl.clear();}
        inline void softClear(){impl.softClear();}
        inline void expand(const arenaSize_t& newSize){impl.expand(newSize);}
        
        inline const T& get(const arenaSize_t& index){return *impl.template get<T>(index); }
        inline const T& getFromBack(const arenaSize_t& indexFromBack){return *impl.template getFromBack<T>(indexFromBack); }
        inline void set(const arenaSize_t& index, const T& value){ (*impl.template get<T>(index)) = value; }
        inline void fill(const T& value){ for (arenaSize_t i = 0; i < impl.maxSize(); i++) (*impl.template get<T>(i)) = value; }

        template<typename... ConstructorArgs> inline void construct_back(ConstructorArgs... args){ * (new (impl.template alloc<T>()) T(args...)); }
        inline void push_back(const T& value){ (*impl.template alloc<T>()) = value; }
        inline T pop_back(){ const T& out = (*impl.get_fromBack(0)); impl.dealloc(1); return out;}
        inline const T& back(){ return (*impl.template getFromBack<T>(0)); }

        [[nodiscard]] inline arenaSize_t maxSize() const {return impl.maxSize();}
        [[nodiscard]] inline arenaSize_t length() const {return impl.length();}
    private:
        ULL<InsideArenaType, T> impl;
    };
}
