//
// Created by nickberryman on 10/12/25.
//
module;
#include <concepts>
export module SG_Pathfind:BaseHashMap;
import SG_Allocator;
import SG_Grid;
import Logger;


export namespace SG_Pathfind::HashMap {
        template <typename T, typename mapsTo>
        concept GridHashmap_c  = requires(T t, SG_Grid::Point p, mapsTo v_t)
        {
            {t.insert(p, v_t)};
            {t.get(p)} -> std::convertible_to<mapsTo>;
            {t.clear()};
        };

        template <typename T>
        concept GridHashset_c  = requires(T t, SG_Grid::Point p)
        {
            {t.insert(p)};
            {t.contains(p)} -> std::same_as<bool>;
            {t.clear()};
        };
    }

