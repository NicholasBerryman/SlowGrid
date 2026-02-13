//
// Created by nickberryman on 9/12/25.
//
module;
#include "Logger.h"

export module SG_Grid:RuntimeSizeGrid;
import :Point;
import :BaseGrid;
import :FullGrid;
import SG_GridConfigs;
import SG_Allocator;
import Logger;

export namespace SG_Grid {
    /**
     * @brief Grid to store arbitrarily typed data, without knowing the size of the grid at compile-time
     * @tparam T Type to store in squares
     * @tparam useBitfield True if a compressed representation for bools should be used (slower, but 8x smaller in memory - requires bool T)
     */
    template <typename T, bool useBitfield = false, bool is2Power = false>
    class RuntimeSizeGrid : private BaseGrid<T> {
        static_assert(!useBitfield | std::is_same_v<T, bool>, "Bitfields can only be used with booleans");
    public:
        template<typename InsideArenaType>
        requires std::is_base_of_v<SG_Allocator::BaseArena, InsideArenaType>
        RuntimeSizeGrid(InsideArenaType& arena, const coordinate_t& width, const coordinate_t& height) : impl(arena,width,height){}

        template<typename InsideArenaType>
        requires std::is_base_of_v<SG_Allocator::BaseArena, InsideArenaType>
        RuntimeSizeGrid(InsideArenaType& arena, const bool& dummyA, const bool& dummyB, const coordinate_t& width, const coordinate_t& height) : impl(arena,width,height){} //Constructor to conform to SparseGrid compatibility

        inline T& get(const Point& at) requires (!useBitfield)  {return impl.get(at);}
        inline T get(const Point& at) requires (useBitfield)  {return impl.get(at);}

        template <typename... ConstructorArgs> inline T& construct(const Point& at, ConstructorArgs&&... args) requires (!useBitfield) { return impl.construct(at, args...);}
        inline void set(const Point& at, const T& value) {impl.set(at, value);}
        [[nodiscard]] inline const coordinate_t& width()const {return impl.width();}
        [[nodiscard]] inline const coordinate_t& height() const {return impl.height();}
        inline void fill(const T& value){impl.fill(value);}
        inline void fill_memset(const char& value){impl.fill_memset(value);}
        typedef T value_type;

        constexpr static bool isBitfieldGrid = useBitfield;

    private:
        FullGrid<T,0,0, useBitfield, is2Power> impl;
    };
}
