//
// Created by nickberryman on 10/12/25.
//

export module SG_Pathfind:BaseHashSet;
import SG_Grid;
import Logger;

// TODO figure out/remember how to index uniquely index grid squares within radius -> use that as the has function



export namespace SG_Pathfind {
    namespace HashSet {
        /**
         * @brief Interface for hash sets. Should only be inherited from, never used directly
         *
         */
        template<typename T, typename hash_t>
        class BaseHashset {
        public:
            inline void insert(const T& value ) {Logging::assert_except(0); }
            inline bool contains(const T& value ) {Logging::assert_except(0); return 0;}
            inline bool remove(const SG_Grid::Point& value ) {Logging::assert_except(0); return 0;}
            inline const hash_t& calcHash(const T& toHash) {Logging::assert_except(0); }


        protected:
            BaseHashset() = default;
        };
    }
}
