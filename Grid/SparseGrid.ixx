//
// Created by nickberryman on 9/12/25.
//
module;
#include "Logger.h"
#include <type_traits>
#include <iostream>
#include <utility>

export module SG_Grid:SparseGrid;
import :Point;
import :BaseGrid;
import :RuntimeSizeGrid;
import SG_GridConfigs;
import SG_Allocator;
import Logger;

export namespace SG_Grid {
    template <typename T>
    concept Chunk_c  = requires(T t, Point p, char c, typename T::value_type v)
    {
        typename T::value_type;
        {t.get(p)} -> std::convertible_to<typename T::value_type>;
        {t.set(p, v)};
        {t.width()} -> std::convertible_to<u_coordinate_t>;
        {t.height()} -> std::convertible_to<u_coordinate_t>;
        {t.fill(v)};
        {t.fill_memset(c)};
    };

    /**
     * @brief Grid to sparsely store arbitrary type data.
     * @tparam Chunk_T Type to use for chunks (sub-grids within this grid)
     * @tparam width_ width of grid in squares (not chunks) - try to keep width and height powers of 2 for performance.
     * @tparam height_ height of grid in squares (not chunks) - try to keep width and height powers of 2 for performance.
     */
    template<Chunk_c Chunk_T, u_coordinate_t width_, u_coordinate_t height_>
    class SparseGrid{
        static_assert(width_ > 0 && height_ > 0 && Chunk_T::compileTimeWidth() > 0, "Width and height must be positive and known at compile time (including for inner chunks)");
        static_assert(BaseGrid_c<Chunk_T>);
    public:
        SparseGrid(){}

        #define SG_SPARSE_GRID_GET \
            LOGGER_ASSERT_EXCEPT(at.x() >= 0 && at.y() >= 0 && at.x() < width_ && at.y() < height_); \
            Chunk_T& chunk = *impl[at.x()/Chunk_T::compileTimeWidth()][at.y()/Chunk_T::compileTimeHeight()]; \
            Point inChunk = Point(at.x()%Chunk_T::compileTimeWidth(), at.y()%Chunk_T::compileTimeHeight()); \
            return chunk.get(inChunk);

        inline const Chunk_T::value_type& get(const Point& at) const requires (!Chunk_T::isBitfieldGrid) { SG_SPARSE_GRID_GET }
        inline Chunk_T::value_type get(const Point& at) const requires (Chunk_T::isBitfieldGrid) { SG_SPARSE_GRID_GET }
        inline Chunk_T::value_type& get(const Point& at) requires (!Chunk_T::isBitfieldGrid) {return const_cast<Chunk_T::value_type&>(std::as_const(*this).get(at));}
        #undef SG_SPARSE_GRID_GET

        inline void set(const Point& at, const Chunk_T::value_type& value) {
            LOGGER_ASSERT_EXCEPT(at.x() >= 0 && at.y() >= 0 && at.x() < width_ && at.y() < height_);
            Chunk_T& chunk = *impl[at.x()/Chunk_T::compileTimeWidth()][at.y()/Chunk_T::compileTimeHeight()];
            Point inChunk = Point(at.x()%Chunk_T::compileTimeWidth(), at.y()%Chunk_T::compileTimeHeight());
            chunk.set(inChunk, value);
        }
        static constexpr u_coordinate_t width(){ return width_;}
        static constexpr u_coordinate_t height(){ return height_;}
        typedef Chunk_T::value_type value_type;

        inline void fill_memset(const Point& at, const char& to) { get(at).fill_memset(to); }

        static constexpr u_coordinate_t chunksWide(){ return width_/Chunk_T::compileTimeWidth()+(width_%Chunk_T::compileTimeWidth()>1);}
        static constexpr u_coordinate_t chunksHigh(){ return height_/Chunk_T::compileTimeHeight()+(height_%Chunk_T::compileTimeHeight()>1);}
        inline void setChunk(const Point& at, Chunk_T* chunk) {
            LOGGER_ASSERT_EXCEPT(at.x() >= 0 && at.y() >= 0 && at.x() < width_ && at.y() < height_);
            impl[at.x()][at.y()] = chunk;
        }
        inline const Chunk_T* getChunk(const Point& at) const {
            LOGGER_ASSERT_EXCEPT(at.x() >= 0 && at.y() >= 0 && at.x() < width_ && at.y() < height_);
            return impl[at.x()][at.y()];
        }
        inline Chunk_T* getChunk(const Point& at) {return const_cast<Chunk_T::value_type&>(std::as_const(*this).get(at));}
    private:
        Chunk_T* impl[chunksWide()][chunksHigh()]; //TODO Double-check that this actually works and isn't relying on garbage values and/or undefined behaviour
    };
    /*
    static_assert(BaseGrid_c<SparseGrid<FullGrid<char,0,0>,3,3>>);
    static_assert(BaseGrid_c<SparseGrid<FullGrid<bool,0,0>,3,3>>);
    static_assert(ConstructingGrid_c<SparseGrid<FullGrid<char,0,0>,3,3>>);
    static_assert(ConstructingGrid_c<SparseGrid<FullGrid<bool,0,0>,3,3>>);
    */
    //TODO make this satisfy the grid concepts
    //  -> make a 'fill_memset' that sets a default value, and fill_memsets every loaded square -> retrieve that if we try to get at a non-loaded chunk
    //  -> make a 'fill' that sets values in every loaded square, and also sets the default value

    /**
     * @brief Grid to sparsely store arbitrary type data, where grid size is not known at compile-time. Supergrid automatically rounded to power of 2 width/height for performance.
     * @tparam Chunk_T Type to use for chunks (sub-grids within this grid)
    */
    template <Chunk_c Chunk_T,typename InsideArenaType>
    class SparseRuntimeGrid {
        static_assert(SG_Allocator::BaseArena_c<InsideArenaType, Chunk_T, u_coordinate_t, u_coordinate_t>);
    public:
        constexpr static bool is2Power = Chunk_T::PowerOf2;

        SparseRuntimeGrid(InsideArenaType& arena, const u_coordinate_t& ChunksWide, const u_coordinate_t& ChunksHigh, const u_coordinate_t& chunkWidth, const u_coordinate_t& chunkHeight) :
        arena(arena),
        impl(arena, ChunksWide, ChunksHigh),
        chunkWidth(chunkWidth),
        chunkHeight(chunkHeight),
        widthDivisor(std::bit_width(this->chunkWidth)-1),
        heightDivisor(std::bit_width(this->chunkHeight)-1) {
            if constexpr (is2Power) LOGGER_ASSERT_EXCEPT((chunkWidth && !(chunkWidth & (chunkWidth - 1))) && (chunkHeight && !(chunkHeight & (chunkHeight - 1))));
        }

        #define SG_GRID_chunkAt_base \
            auto& chunk = impl.get(Point(at.x()/chunkWidth,at.y()/chunkHeight)); \
            Point inChunk = Point(at.x()%chunkWidth, at.y()%chunkHeight);
        #define SG_GRID_chunkAt_shift \
            auto& chunk = impl.get(Point(at.x() >> widthDivisor,at.y() >> heightDivisor)); \
            Point inChunk = Point(static_cast<std::make_unsigned_t<u_coordinate_t>>(at.x()) & (chunkWidth-1), static_cast<std::make_unsigned_t<u_coordinate_t>>(at.y()) & (chunkHeight-1));

        #define SG_SPARSE_GRID_SET_GET(act) \
            LOGGER_ASSERT_EXCEPT(at.x() >= 0 && at.y() >= 0 && at.x() < width() && at.y() < width()); \
            if constexpr (!is2Power) { \
                SG_GRID_chunkAt_base; \
                return act; \
            } else { \
                SG_GRID_chunkAt_base; \
                return act; \
            }

        inline const Chunk_T::value_type& get(const Point& at) const requires (!Chunk_T::isBitfieldGrid) { SG_SPARSE_GRID_SET_GET(chunk.get(inChunk)) }
        inline Chunk_T::value_type get(const Point& at) const requires (Chunk_T::isBitfieldGrid) { SG_SPARSE_GRID_SET_GET(chunk.get(inChunk)) }
        inline Chunk_T::value_type& get(const Point& at) requires (!Chunk_T::isBitfieldGrid) { return const_cast<Chunk_T::value_type&>(std::as_const(*this).get(at)); }

        inline void set(const Point& at, const Chunk_T::value_type& value) { SG_SPARSE_GRID_SET_GET(chunk.set(inChunk, value)) }
        #undef SG_SPARSE_GRID_SET_GET

        [[nodiscard]] inline SG_Grid::Point chunkLocation(const Point& at) const {
            if constexpr (!is2Power) return Point(at.x()/chunkWidth,at.y()/chunkHeight);
            else  return Point(at.x() >> widthDivisor,at.y() >> heightDivisor);
        }

        [[nodiscard]] inline u_coordinate_t width() const{ return chunkWidth * chunksWide(); }
        [[nodiscard]] inline u_coordinate_t height() const { return chunkHeight * chunksHigh(); }
        typedef Chunk_T::value_type value_type;

        [[nodiscard]] inline const u_coordinate_t& chunksWide() const { return impl.width();}
        [[nodiscard]] inline const u_coordinate_t& chunksHigh() const { return impl.height(); }

        inline void fill_memset(const Point& at, const char& to) { get(at).fill_memset(to); }

        template <bool clear = false> inline void loadChunk(const Point& at) {
            LOGGER_ASSERT_EXCEPT(at.x() >= 0 && at.y() >= 0 && at.x() < width() && at.y() < width());
            auto& out = impl.construct(at, arena, chunkWidth, chunkHeight);
            if constexpr (clear) out.fill_memset(0);
        }
        inline Chunk_T& getChunk(const Point& at) {
            LOGGER_ASSERT_EXCEPT(at.x() >= 0 && at.y() >= 0 && at.x() < width() && at.y() < width());
            return impl.get(at);
        }
    private:
        InsideArenaType& arena;
        RuntimeSizeGrid<Chunk_T, false, true> impl;
        const u_coordinate_t chunkWidth;
        const u_coordinate_t chunkHeight;

        const u_coordinate_t widthDivisor;
        const u_coordinate_t heightDivisor;
    };
    /*
    static_assert(BaseGrid_c<SparseRuntimeGrid<RuntimeSizeGrid<char>, SG_Allocator::defaultArena<>>>);
    static_assert(BaseGrid_c<SparseRuntimeGrid<RuntimeSizeGrid<bool>, SG_Allocator::defaultArena<>>>);
    static_assert(ConstructingGrid_c<SparseRuntimeGrid<RuntimeSizeGrid<char>, SG_Allocator::defaultArena<>>>);
    static_assert(ConstructingGrid_c<SparseRuntimeGrid<RuntimeSizeGrid<bool>, SG_Allocator::defaultArena<>>>);
    */
    //TODO same as above
}
