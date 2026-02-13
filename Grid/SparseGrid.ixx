//
// Created by nickberryman on 9/12/25.
//
module;
#include "Logger.h"
#include <type_traits>
#include <iostream>

export module SG_Grid:SparseGrid;
import :Point;
import :BaseGrid;
import :RuntimeSizeGrid;
import SG_GridConfigs;
import SG_Allocator;
import Logger;

//TODO add a 'fill_memset' method per chunk
export namespace SG_Grid {
    /**
     * @brief Grid to sparsely store arbitrary type data.
     * @tparam Chunk_T Type to use for chunks (sub-grids within this grid)
     * @tparam width_ width of grid in squares (not chunks) - try to keep width and height powers of 2 for performance.
     * @tparam height_ height of grid in squares (not chunks) - try to keep width and height powers of 2 for performance.
     */
    template<typename Chunk_T, coordinate_t width_, coordinate_t height_>
	requires std::is_base_of_v<BaseGrid<typename Chunk_T::value_type>, Chunk_T>
    class SparseGrid : private BaseGrid<typename Chunk_T::value_type>{
        static_assert(width_ > 0 && height_ > 0 && Chunk_T::compileTimeWidth() > 0, "Width and height must be positive and known at compile time (including for inner chunks)");
    public:
        SparseGrid(){}
        inline Chunk_T::value_type& get(const Point& at) requires (!Chunk_T::isBitfieldGrid) {
            LOGGER_ASSERT_EXCEPT(at.x() >= 0 && at.y() >= 0 && at.x() < width_ && at.y() < height_);
            Chunk_T& chunk = *impl[at.x()/Chunk_T::compileTimeWidth()][at.y()/Chunk_T::compileTimeHeight()];
            Point inChunk = Point(at.x()%Chunk_T::compileTimeWidth(), at.y()%Chunk_T::compileTimeHeight());
            return chunk.get(inChunk);
        }
        inline Chunk_T::value_type get(const Point& at) requires (Chunk_T::isBitfieldGrid) {
            LOGGER_ASSERT_EXCEPT(at.x() >= 0 && at.y() >= 0 && at.x() < width_ && at.y() < height_);
            Chunk_T& chunk = *impl[at.x()/Chunk_T::compileTimeWidth()][at.y()/Chunk_T::compileTimeHeight()];
            Point inChunk = Point(at.x()%Chunk_T::compileTimeWidth(), at.y()%Chunk_T::compileTimeHeight());
            return chunk.get(inChunk);
        }
        inline void set(const Point& at, const Chunk_T::value_type& value) {
            LOGGER_ASSERT_EXCEPT(at.x() >= 0 && at.y() >= 0 && at.x() < width_ && at.y() < height_);
            Chunk_T& chunk = *impl[at.x()/Chunk_T::compileTimeWidth()][at.y()/Chunk_T::compileTimeHeight()];
            Point inChunk = Point(at.x()%Chunk_T::compileTimeWidth(), at.y()%Chunk_T::compileTimeHeight());
            chunk.set(inChunk, value);
        }
        static constexpr coordinate_t width(){ return width_;}
        static constexpr coordinate_t height(){ return height_;}
        typedef Chunk_T::value_type value_type;

        static constexpr coordinate_t chunksWide(){ return width_/Chunk_T::compileTimeWidth()+(width_%Chunk_T::compileTimeWidth()>1);}
        static constexpr coordinate_t chunksHigh(){ return height_/Chunk_T::compileTimeHeight()+(height_%Chunk_T::compileTimeHeight()>1);}
        inline void setChunk(const Point& at, Chunk_T* chunk) {
            LOGGER_ASSERT_EXCEPT(at.x() >= 0 && at.y() >= 0 && at.x() < width_ && at.y() < height_);
            impl[at.x()][at.y()] = chunk;
        }
        inline Chunk_T* getChunk(const Point& at) {
            LOGGER_ASSERT_EXCEPT(at.x() >= 0 && at.y() >= 0 && at.x() < width_ && at.y() < height_);
            return impl[at.x()][at.y()];
        }
    private:
        Chunk_T* impl[chunksWide()][chunksHigh()];
    };


    /**
     * @brief Grid to sparsely store arbitrary type data.
     * @tparam Chunk_T Type to use for chunks (sub-grids within this grid)
     * @tparam width_ width of grid in squares (not chunks) - try to keep width and height powers of 2 for performance.
     * @tparam height_ height of grid in squares (not chunks) - try to keep width and height powers of 2 for performance.
    */
    template <typename InsideArenaType,typename Chunk_T>
	requires (std::is_base_of_v<BaseGrid<typename Chunk_T::value_type>, Chunk_T> && std::is_base_of_v<SG_Allocator::BaseArena, InsideArenaType>)
    class SparseRuntimeGrid : private BaseGrid<typename Chunk_T::value_type>{
    public:
        SparseRuntimeGrid(InsideArenaType& arena, const coordinate_t& ChunksWide, const coordinate_t& ChunksHigh, const coordinate_t& chunkWidth, const coordinate_t& chunkHeight) :
        arena(arena),
        impl(arena, ChunksWide, ChunksHigh),
        chunkWidth(chunkWidth),
        chunkHeight(chunkHeight) {}
        inline Chunk_T::value_type& get(const Point& at) requires (!Chunk_T::isBitfieldGrid) {
            LOGGER_ASSERT_EXCEPT(at.x() >= 0 && at.y() >= 0 && at.x() < width() && at.y() < width());
            Chunk_T& chunk = impl.get(Point(at.x()/chunkWidth,at.y()/chunkHeight));
            Point inChunk = Point(at.x()%chunkWidth, at.y()%chunkHeight);
            return chunk.get(inChunk);
        }
        inline Chunk_T::value_type get(const Point& at) requires (Chunk_T::isBitfieldGrid) {
            LOGGER_ASSERT_EXCEPT(at.x() >= 0 && at.y() >= 0 && at.x() < width() && at.y() < width());
            Chunk_T& chunk = impl.get(Point(at.x()/chunkWidth,at.y()/chunkHeight));
            Point inChunk = Point(at.x()%chunkWidth, at.y()%chunkHeight);
            return chunk.get(inChunk);
        }
        inline void set(const Point& at, const Chunk_T::value_type& value) {
            LOGGER_ASSERT_EXCEPT(at.x() >= 0 && at.y() >= 0 && at.x() < width() && at.y() < width());
            Chunk_T& chunk = impl.get(Point(at.x()/chunkWidth,at.y()/chunkHeight));
            Point inChunk = Point(at.x()%chunkWidth, at.y()%chunkHeight);
            chunk.set(inChunk, value);
        }
        [[nodiscard]] inline coordinate_t width() const{ return chunkWidth * chunksWide(); }
        [[nodiscard]] inline coordinate_t height() const { return chunkHeight * chunksHigh(); }
        typedef Chunk_T::value_type value_type;

        [[nodiscard]] inline const coordinate_t& chunksWide() const { return impl.width();}
        [[nodiscard]] inline const coordinate_t& chunksHigh() const { return impl.height(); }

        inline void loadChunk(const Point& at) {
            LOGGER_ASSERT_EXCEPT(at.x() >= 0 && at.y() >= 0 && at.x() < width() && at.y() < width());
            impl.construct(at, arena, chunkWidth, chunkHeight);
        }
        inline Chunk_T& getChunk(const Point& at) {
            LOGGER_ASSERT_EXCEPT(at.x() >= 0 && at.y() >= 0 && at.x() < width() && at.y() < width());
            return impl.get(at);
        }
    private:
        InsideArenaType& arena;
        RuntimeSizeGrid<Chunk_T> impl;
        const coordinate_t chunkWidth;
        const coordinate_t chunkHeight;
    };
}
