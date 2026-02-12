//
// Created by nickberryman on 1/12/25.
//

module;
#include <type_traits>
#include "Logger.h"

export module SG_Allocator:UnrolledLinkedList2;
import :UnrolledLinkedList;
import :BaseArena;
import :PseudoArena;
import SG_AllocatorConfigs;
import Logger;

export namespace SG_Allocator {
    /**
     * @brief Homogenous unrolled doubly-linked list, with arena allocation support. Guarantees tightly packed allocations (unlike heterogeneous version)
     *
     * @tparam InsideArenaType Arena type to allocate blocks into (Does not accept PseudoArena)
     * @tparam T Data type to use for elements
     */
    template<typename InsideArenaType, typename T>
    requires std::is_base_of_v<BaseArena, InsideArenaType> && (!(std::is_base_of_v<PseudoArena, InsideArenaType>))
    class ULL2 {
    public:
        ULL2(InsideArenaType& arena, const arenaSize_t initialSize):
            impl(arena, initialSize){}

        inline void clear(){impl.clear();}
        inline void softClear(){impl.softClear();}
        inline void expand(const arenaSize_t& newSize){impl.expand(newSize);}
        
        inline T& get(const arenaSize_t& index){return *impl.template get<T>(index); }
        inline T& getFromBack(const arenaSize_t& indexFromBack){return *impl.template getFromBack<T>(indexFromBack); }
        inline void set(const arenaSize_t& index, const T& value){ (*impl.template get<T>(index)) = value; }
        inline void fill(const T& value){ for (arenaSize_t i = 0; i < impl.maxSize(); i++) (*impl.template get<T>(i)) = value; }

        template<typename... ConstructorArgs> inline T& construct_back(ConstructorArgs&&... args) { return * (new (impl.template alloc<T>()) T(args...)); }
        template<typename... ConstructorArgs> inline T* alloc_back(const arenaSize_t& count) { return impl.template allocArray<T>(count); }
        inline void push_back(const T& value){ (*impl.template alloc<T>()) = value; }
        inline T pop_back(){ const T& out = (*impl.get_fromBack(0)); impl.dealloc(1); return out;}
        inline const T& back(){ return (*impl.template getFromBack<T>(0)); }

        [[nodiscard]] inline arenaSize_t maxSize() const {return impl.maxSize();}
        [[nodiscard]] inline arenaSize_t length() const {return impl.length();}
    private:
        ULL<InsideArenaType, T> impl;
    };
}
