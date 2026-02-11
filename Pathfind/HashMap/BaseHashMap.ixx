//
// Created by nickberryman on 10/12/25.
//

export module SG_Pathfind:BaseHashMap;
import SG_Grid;
import Logger;

export namespace SG_Pathfind {
    namespace HashMap {
        /**
         * @brief Interface for hash sets. Should only be inherited from, never used directly
         *
         */
        template<typename key_t, typename value_t, typename hash_t>
        class BaseHashMap {
        public:
            inline void insert(const key_t& key, const value_t& value ) {Logging::assert_except(0); }
            inline bool contains(const key_t& key) {Logging::assert_except(0); return 0;}
            inline value_t& get(const key_t& key) {Logging::assert_except(0); return 0;}
            inline bool remove(const key_t& key) {Logging::assert_except(0); return 0;}
            inline const hash_t& calcHash(const key_t& key) {Logging::assert_except(0); }


        protected:
            BaseHashMap() = default;
        };
    }
}
