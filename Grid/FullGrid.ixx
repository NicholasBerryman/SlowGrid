//
// Created by nickberryman on 9/12/25.
//
module;
#include "Logger.h"
#include <cstring>

export module SG_Grid:FullGrid;
import :Point;
import :BaseGrid;
import SG_Allocator;
import SG_GridConfigs;
import Logger;

export namespace SG_Grid {
    /**
     * @brief Grid to store arbitrary typed data using a single bit per square.
     * @tparam width_ width of grid in squares - try to keep width and height powers of 2 for performance.
     * @tparam height_ height of grid in squares - try to keep width and height powers of 2 for performance.
     */
    template<typename T, u_coordinate_t width_, u_coordinate_t height_, bool isBitfield = false, bool is2Power = false>
    class FullGrid : private BaseGrid<T> {
        static_assert((width_ > 0 && height_ > 0) || (width_ == 0 && height_ == 0), "Width and height must both be positive or zero");
        static_assert(!isBitfield || std::is_same_v<bool, T>, "Bitfield only supported for bools");
    public:
        FullGrid() requires (width_ > 0) :
            width_var(width_),
            height_var(height_) {}

        template<typename InsideArenaType> requires std::is_base_of_v<SG_Allocator::BaseArena, InsideArenaType>
        explicit FullGrid(InsideArenaType& arena, const u_coordinate_t& width, const u_coordinate_t& height) requires (width_ == 0 && is2Power) :
            impl(arena,(internalWidth(width) * height)),
            width_var(width),
            height_var(height),
            widthMult(std::bit_width((width_var))-1),
            heightMult(std::bit_width((height_var))-1)
        { LOGGER_ASSERT_EXCEPT(width > 0 && height > 0);}

        template<typename InsideArenaType> requires std::is_base_of_v<SG_Allocator::BaseArena, InsideArenaType>
        explicit FullGrid(InsideArenaType& arena, const u_coordinate_t& width, const u_coordinate_t& height) requires (width_ == 0 && !is2Power) :
            impl(arena,(internalWidth(width) * height)),
            width_var(width),
            height_var(height)
        { LOGGER_ASSERT_EXCEPT(width > 0 && height > 0);}

        inline T get(const Point& at) requires (isBitfield) {
            LOGGER_ASSERT_EXCEPT(at.x() >= 0 && at.y() >= 0 && at.x() < width() && at.y() < height());
            return (smartFind(at.x(),at.y()) >> (at.x()%8)) & 1;
        }
        inline T& get(const Point& at) requires (!isBitfield) {
            LOGGER_ASSERT_EXCEPT(at.x() >= 0 && at.y() >= 0 && at.x() < width() && at.y() < height());
            return smartFind(at.x(),at.y());
        }
        inline void set(const Point& at, const T& value) requires (isBitfield) {
            LOGGER_ASSERT_EXCEPT(at.x() >= 0 && at.y() >= 0 && at.x() < width() && at.y() < height());
            if (value) smartFind(at.x(),at.y()) |= (1 << (at.x()%8));
            else smartFind(at.x(),at.y()) &= ~(1 << (at.x()%8));
        }
        inline void set(const Point& at, const T& value) requires (!isBitfield) {
            LOGGER_ASSERT_EXCEPT(at.x() >= 0 && at.y() >= 0 && at.x() < width() && at.y() < height());
            smartFind(at.x(),at.y()) = value;
        }
        inline void fill(const T& value) requires (!isBitfield){
            for (int x = 0; x < internalWidth(width()); x++)
                for (int y = 0; y < height(); y++)
                    find(x,y) = value;
        }
        inline void fill(const T& value) requires (isBitfield){
            for (int x = 0; x < internalWidth(width()); x++)
                for (int y = 0; y < height(); y++) {
                    if (value) find(x,y) = ~0;
                    else find(x,y) = 0;
                }
        }
        inline void fill_memset(const char& toFill){
            if constexpr (width_ != 0) std::memset(impl, toFill, height_ * compileTimeInternalWidth());
            else std::memset(impl.impl(), toFill, height() * internalWidth(width()) * sizeof(internalT));
        }

        template <typename... ConstructorArgs> inline T& construct(const Point& at, ConstructorArgs&&... args) requires (!isBitfield) {
            return *new (&get(at)) T(args...);
        }

        [[nodiscard]] const u_coordinate_t& width() const { return width_var; }
        [[nodiscard]] const u_coordinate_t& height() const { return height_var; }

        static constexpr u_coordinate_t compileTimeWidth(){ return width_;}
        static constexpr u_coordinate_t compileTimeHeight(){ return height_;}
        typedef T value_type;
        constexpr static bool isBitfieldGrid = isBitfield;

    private:
        static consteval u_coordinate_t compileTimeInternalWidth() { if (isBitfield) return width_/8+(width_%8 > 0); return width_;}
        typedef std::conditional_t<isBitfield, char, T> internalT;
        struct empty{};

        std::conditional_t<width_ != 0, std::array<std::array<internalT,height_>,compileTimeInternalWidth()>, SG_Allocator::RuntimeArray<internalT>> impl;
        const u_coordinate_t width_var;
        const u_coordinate_t height_var;
        [[no_unique_address]] std::conditional_t<is2Power, const u_coordinate_t, empty> widthMult;
        [[no_unique_address]] std::conditional_t<is2Power, const u_coordinate_t, empty> heightMult;

        inline internalT& find(const u_coordinate_t& x, const u_coordinate_t& y) requires (width_ > 0) { return impl[x][y]; }
        inline internalT& find(const u_coordinate_t& x, const u_coordinate_t& y) requires (width_ == 0) {
            if constexpr (!is2Power) return impl[x+y*internalWidth(width())];
            else                     return impl[x+(y<<widthMult)];
        }

        inline internalT& smartFind(const u_coordinate_t& x, const u_coordinate_t& y) requires (!isBitfield) { return find(x,y); }
        inline internalT& smartFind(const u_coordinate_t& x, const u_coordinate_t& y) requires (isBitfield) { return find(x/8,y); }

        [[nodiscard]] static inline u_coordinate_t internalWidth(const u_coordinate_t& x) requires isBitfield { return x/8+(x%8 > 0); }
        [[nodiscard]] static inline const u_coordinate_t& internalWidth(const u_coordinate_t& x) requires (!isBitfield) { return x; }
    };
}
