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
		template<typename T> static inline T* alloc(){return reinterpret_cast<T *>(new char[sizeof(T)]);};
		template<typename T> static inline T* allocArray(const arenaSize_t& arrayLength){return reinterpret_cast<T *>(new char[sizeof(T) * arrayLength]);};
		template<typename T, typename... ConstructorArgs> static inline T* allocConstruct(ConstructorArgs&&... args){return new T(args...);};
		template<typename T, typename... ConstructorArgs> static inline T* allocConstructArray(const arenaSize_t& arrayLength, ConstructorArgs&&... args) {
			auto out = new T[arrayLength];
			for (auto i = 0; i < arrayLength; i++) new (&out[i]) T(args...);
			return out;
		};
		inline void static sublifetime_open(){Logging::assert_except(0);};
		inline void static sublifetime_rollback(){Logging::assert_except(0);};
		inline void static sublifetime_softRollback(){Logging::assert_except(0);};
		inline void static clear(){Logging::assert_except(0);};
		inline void static softClear(){Logging::assert_except(0);};
		inline void static expand(arenaSize_t newSize){};
		inline arenaSize_t static maxSize(){Logging::assert_except(0);return 0;};
		inline arenaSize_t static usedSpace(){Logging::assert_except(0);return 0;};

		template<typename T> inline void static softDelete(T* toDelete){delete toDelete;};
		template<typename T> inline void static softDeleteArray(T* toDelete){delete[] toDelete;};

	};
}
