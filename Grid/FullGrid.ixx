//
// Created by nickberryman on 9/12/25.
//
module;
#include "Logger.h"
export module SG_Grid:FullGrid;
import :Point;
import :BaseGrid;
import SG_GridConfigs;
import Logger;

export namespace SG_Grid {
    /**
     * @brief Grid to store arbitrary type data.
     * @tparam width_ width of grid in squares - try to keep width and height powers of 2 for performance.
     * @tparam height_ height of grid in squares - try to keep width and height powers of 2 for performance.
     */
    template<typename T, coordinate_t width_, coordinate_t height_>
    class FullGrid : private BaseGrid<T>{
        static_assert(width_ > 0 && height_ > 0, "Width and height must be positive");
    public:
        FullGrid(){}
        inline const T& get(const Point& at){
            LOGGER_ASSERT_EXCEPT(at.x() >= 0 && at.y() >= 0 && at.x() < width_ && at.y() < height_);
            return impl[at.x()][at.y()];
        }
        inline void set(const Point& at, const T& value) {
            LOGGER_ASSERT_EXCEPT(at.x() >= 0 && at.y() >= 0 && at.x() < width_ && at.y() < height_);
            impl[at.x()][at.y()] = value;
        }
        inline void fill(const T& value){
            for (int x = 0; x < width_; x++)
                for (int y = 0; y < height_; y++)
                    impl[x][y] = value;
        }
        
        
        static constexpr coordinate_t width(){ return width_;}
        static constexpr coordinate_t height(){ return height_;}
        typedef T value_type;

    private:
        T impl[width_][height_];
    };
}
