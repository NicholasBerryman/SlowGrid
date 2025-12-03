//
// Created by nickberryman on 1/12/25.
//
export module SG_Allocator:BaseArena;
import SG_AllocatorConfigs;
import Logger;

export namespace SG_Allocator {
    /**
     * @brief Base class for arena allocators. Should only be inhereted from, never used directly
     * 
     */
    class BaseArena {
    public:
        template<typename T> inline T* alloc(){Logging::assert_except(0); return nullptr;};
        template<typename T> inline T* allocArray(arenaSize_t arrayLength){Logging::assert_except(0); return nullptr;};
		template<typename T, typename... ConstructorArgs> inline T* allocConstruct(ConstructorArgs... args){Logging::assert_except(0); return nullptr;};
		template<typename T, typename... ConstructorArgs> inline T* allocConstructArray(arenaSize_t arrayLength, ConstructorArgs... args){Logging::assert_except(0); return nullptr;};
        inline void sublifetime_open(){Logging::assert_except(0);};
        inline void sublifetime_rollback(){Logging::assert_except(0);};
        inline void sublifetime_softRollback(){Logging::assert_except(0);};
        inline void clear(){Logging::assert_except(0);};
        inline void softClear(){Logging::assert_except(0);};
        inline void expand(arenaSize_t newSize){Logging::assert_except(0);};
        inline arenaSize_t maxSize(){Logging::assert_except(0);return 0;};
        inline arenaSize_t usedSpace(){Logging::assert_except(0);return 0;};

        template<typename T> inline void softDelete(T* toDelete){Logging::assert_except(0);};
        template<typename T> inline void softDeleteArray(T* toDelete){Logging::assert_except(0);};

    protected:
        BaseArena() = default;
    };
}
