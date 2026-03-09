//
// Created by nickberryman on 10/12/25.
//

module;
#include <cmath>
#include <type_traits>
#include "Logger.h"
#include <cstdint>
#include <unordered_map>
#include <utility>

export module SG_Pathfind:STDHashMap;
import :BaseHashMap;
import SG_Grid;
import Logger;

export namespace SG_Pathfind::HashMap {
    template<typename insideArena_t, typename value_t = bool, bool _ = false, int __ = 0, int ___ = 0>
    class STDHashMap : private BaseHashMap<SG_Grid::Point, value_t, SG_Grid::Point>{
    private:    
    public: 
        template <typename pathfindGrid_t>
        inline STDHashMap(insideArena_t& arena, const pathfindGrid_t& within, const SG_Grid::Point& centrePoint, const SG_Grid::coordinate_t& distance):
            hashWidth(within.width())
        {}

        inline void insert(const SG_Grid::Point& key) requires (std::is_same_v<value_t, bool>) { 
            map[calcHash(key)] = true;
        }
        inline void insert(const SG_Grid::Point& key, const value_t& val) requires (!std::is_same_v<value_t, bool>){
            map[calcHash(key)] = val;
        }

        [[nodiscard]] inline bool contains(const SG_Grid::Point& key ) const {
            return(map.contains(calcHash(key)));
        }

        inline const value_t& get(const SG_Grid::Point& key ) const requires (!(std::is_same_v<value_t, bool>)) {
            return(map.find(calcHash(key))->second);
        }
        inline value_t& get(const SG_Grid::Point& key ) requires (!(std::is_same_v<value_t, bool>)) { return const_cast<value_t &>(std::as_const(*this).get(key)); }

        inline void remove(const SG_Grid::Point& key ) { 
            map.erase(calcHash(key));
        }
        
        [[nodiscard]] inline SG_Grid::u_coordinate_t calcHash(const SG_Grid::Point& toHash) const {
            return(toHash.x() + toHash.y()*hashWidth);
        }
        
        inline void clear() {
            map.clear();
        }

    private:
        std::unordered_map<SG_Grid::coordinate_t, value_t> map;
        SG_Grid::u_coordinate_t hashWidth;
    };
}

