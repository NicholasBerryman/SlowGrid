//
// Created by nickberryman on 2/12/25.
//

module;
export module SG_Allocator:PseudoArena;
import SG_AllocatorConfigs;
import :BaseArena;
import Logger;

export namespace SG_Allocator {
	class PseudoArena : private BaseArena {
	public:
		template<typename T> inline T* alloc(){return (T*) new char[sizeof(T)];};
		template<typename T> inline T* allocArray(arenaSize_t arrayLength){return (T*) new char[sizeof(T)*arrayLength];};
		template<typename T, typename... ConstructorArgs> inline T* allocConstruct(ConstructorArgs... args){return new T(args...);};
		template<typename T, typename... ConstructorArgs> inline T* allocConstructArray(arenaSize_t arrayLength, ConstructorArgs... args) {
			auto out = new T[arrayLength];
			for (auto i = 0; i < arrayLength; i++) new (&out[i]) T(args...);
			return out;
		};
		inline void sublifetime_open(){Logging::assert_except(0);};
		inline void sublifetime_rollback(){Logging::assert_except(0);};
		inline void sublifetime_softRollback(){Logging::assert_except(0);};
		inline void clear(){Logging::assert_except(0);};
		inline void softClear(){Logging::assert_except(0);};
		inline void expand(arenaSize_t newSize){};
		inline arenaSize_t maxSize(){Logging::assert_except(0);return 0;};
		inline arenaSize_t usedSpace(){Logging::assert_except(0);return 0;};

		template<typename T> inline void softDelete(T* toDelete){delete toDelete;};
		template<typename T> inline void softDeleteArray(T* toDelete){delete[] toDelete;};

	};
}
