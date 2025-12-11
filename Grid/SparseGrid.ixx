//
// Created by nickberryman on 9/12/25.
//
module;
#include "Logger.h"
#include <type_traits>
export module SG_Grid:SparseGrid;
import :Point;
import :BaseGrid;
import SG_GridConfigs;
import Logger;

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
        static_assert(width_ > 0 && height_ > 0, "Width and height must be positive");
    public:
        SparseGrid(){}
        inline const Chunk_T::value_type& get(const Point& at){
            LOGGER_ASSERT_EXCEPT(at.x() >= 0 && at.y() >= 0 && at.x() < width_ && at.y() < height_);
            Chunk_T& chunk = *impl[at.x()/Chunk_T::width()][at.y()/Chunk_T::height()];
            Point inChunk = Point(at.x()%Chunk_T::width(), at.y()%Chunk_T::height());
            return chunk.get(inChunk);
        }
        inline void set(const Point& at, const Chunk_T::value_type& value) {
            LOGGER_ASSERT_EXCEPT(at.x() >= 0 && at.y() >= 0 && at.x() < width_ && at.y() < height_);
            Chunk_T& chunk = *impl[at.x()/Chunk_T::width()][at.y()/Chunk_T::height()];
            Point inChunk = Point(at.x()%Chunk_T::width(), at.y()%Chunk_T::height());
            chunk.set(inChunk, value);
        }
        static constexpr coordinate_t width(){ return width_;}
        static constexpr coordinate_t height(){ return height_;}
        typedef Chunk_T::value_type value_type;

        static constexpr coordinate_t chunksWide(){ return width_/Chunk_T::width()+(width_%Chunk_T::width()>1);}
        static constexpr coordinate_t chunksHigh(){ return height_/Chunk_T::height()+(height_%Chunk_T::height()>1);}
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
}
