//
// Created by nickberryman on 10/12/25.
//

module;
#include <type_traits>
#include <algorithm>

export module SG_Pathfind:GridRangeHashSet;
import :BaseHashSet;
import SG_Grid;
import Logger;

// TODO figure out/remember how to index uniquely index grid squares within radius -> use that as the has function



export namespace SG_Pathfind {
    namespace HashSet {
        template<bool isBitfield, typename pathfindGrid_t, typename insideArena_t>
        class GridRangeHashset : private BaseHashset<SG_Grid::Point, SG_Grid::Point>{
        public:        
            inline GridRangeHashset(const SG_Grid::Point& centrePoint, const SG_Grid::coordinate_t& distance, const pathfindGrid_t& within, insideArena_t& arena) :
                origin(std::max(0, centrePoint.x() - distance), std::max(0, centrePoint.y() - distance)),
                arena(arena),
                hashGrid(arena, std::min(pathfindGrid_t::width(), centrePoint.x() + distance) - centrePoint.x(), std::min(pathfindGrid_t::height(), centrePoint.y() + distance) - centrePoint.y())
            {
                hashGrid.fill(0);
            };
            inline void insert(const SG_Grid::Point& value ) {hashGrid.set(value-origin,1);}
            inline const bool& contains(const SG_Grid::Point& value ) {return hashGrid.get(value-origin);}
            inline bool remove(const SG_Grid::Point& value ) {const bool& out = contains(value-origin); hashGrid.set(value-origin, 0); return out;}
            [[nodiscard]] inline SG_Grid::Point calcHash(const SG_Grid::Point& toHash) const {return toHash-origin;}

        private:
            SG_Grid::Point origin;
            insideArena_t& arena;
            SG_Grid::RuntimeSizeGrid<bool, isBitfield> hashGrid;
        };
    }
}
