//
// Created by nickberryman on 9/12/25.
//
module;
#include <concepts>
#include <utility>

export module SG_Grid:BaseGrid;
import :Point;
import SG_GridConfigs;
import Logger;

export namespace SG_Grid {
    template <typename T>
    concept ReadableGrid_c = requires (T t, Point p, typename T::value_type v)
    {
        typename T::value_type;
        {t.get(p)} -> std::convertible_to<typename T::value_type>;
        {t.width()} -> std::convertible_to<u_coordinate_t>;
        {t.height()} -> std::convertible_to<u_coordinate_t>;
    };

    template <typename T>
    concept BaseGrid_c  = requires(T t, Point p, typename T::value_type v, char c)
    {
        typename T::value_type;
        {t.set(p, v)};
        {t.fill(v)};
        {t.fill_memset(c)};
    } && ReadableGrid_c<T>;

    template <typename T, typename... canAllocArgs>
    concept ConstructingGrid_c  = requires(T t, Point p, typename T::value_type v, char c, canAllocArgs... args)
    {
        {t.template construct<canAllocArgs...>(p, std::forward<canAllocArgs...>(args...))};
    } && BaseGrid_c<T>;

}
