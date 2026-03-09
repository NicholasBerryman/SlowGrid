//
// Created by nickberryman on 1/12/25.
//

module;
#include <type_traits>
#include "Logger.h"

export module SG_Allocator:RuntimeArray;
import :BaseArena;
import :PseudoArena;
import SG_AllocatorConfigs;
import Logger;

//TODO make LocalDataStructure equivalents that work like this -> Will speed up bfs etc

export namespace SG_Allocator {
    template <typename T, typename size_T = arenaSize_t>
    class RuntimeArray {
        public:
        template<typename InsideArenaType>
            RuntimeArray(InsideArenaType& arena,const size_T& maxSize):
                impl_(arena.template allocArray<T>(maxSize))
                #ifndef NDEBUG
                ,maxSize_(maxSize)
                #endif
            {}

            #ifndef NDEBUG
            size_T maxSize(){ return maxSize_; }
            #endif

            const T& operator [](size_T i) const {
                LOGGER_ASSERT_EXCEPT(i >= 0 && i < maxSize_);
                return impl_[i];
            }
            T& operator [](size_T i){
                LOGGER_ASSERT_EXCEPT(i >= 0 && i < maxSize_);
                return impl_[i];
            }

            const T& get(size_T i) const {
                LOGGER_ASSERT_EXCEPT(i >= 0 && i < maxSize_);
                return impl_[i];
                }
            T& get(size_T i){
                LOGGER_ASSERT_EXCEPT(i >= 0 && i < maxSize_);
                return impl_[i];
                }

            T* impl() {return impl_;}

        private:
            T* const impl_;
            #ifndef NDEBUG
            size_T maxSize_;
            #endif

    };
}



