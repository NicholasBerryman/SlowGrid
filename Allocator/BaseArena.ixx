//
// Created by nickberryman on 1/12/25.
//
module;
#include <concepts>

export module SG_Allocator:BaseArena;
import SG_AllocatorConfigs;
import Logger;

export namespace SG_Allocator {
    template <typename T, typename canAlloc, typename... canAllocArgs>
    concept BaseArena_c  = requires(T t, canAlloc a, typename T::arenaSize_t siz, canAllocArgs... args)
    {
        typename T::arenaSize_t;
        {t.template alloc<canAlloc>()} -> std::same_as<canAlloc*>;
        {t.template allocArray<canAlloc>(3)} -> std::same_as<canAlloc*>;
        {t.template allocConstruct<canAlloc>(args...)} -> std::same_as<canAlloc*>;
        {t.template allocConstructArray<canAlloc>(3, args...)} -> std::same_as<canAlloc*>;
        {t.sublifetime_open()};
        {t.sublifetime_rollback()};
        {t.sublifetime_softRollback()};
        {t.clear()};
        {t.softClear()};
        {t.expand(siz)};
        {t.maxSize()} -> std::same_as<typename T::arenaSize_t>;
        {t.usedSpace()} -> std::same_as<typename T::arenaSize_t>;
    };

    template <typename T, typename canAlloc, typename... canAllocArgs>
    concept OptionalArena_c  = requires(T t, canAlloc* a)
    {
        {t.softDelete(a)};
        {t.softDeleteArray(a)};
    } && BaseArena_c<T, canAlloc, canAllocArgs...>;

    /**
     * @brief Base class for arena allocators. Should only be inherited from, never used directly
     *
     */
    class BaseArena {
    public:
        using arenaSize_t = defaultArenaSize_t;

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
