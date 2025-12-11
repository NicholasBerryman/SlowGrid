//
// Created by nickberryman on 9/12/25.
//
module;
#include "Logger.h"
export module SG_Grid:BitfieldFullGrid;
import :Point;
import :BaseGrid;
import SG_GridConfigs;
import Logger;

export namespace SG_Grid {
    /**
     * @brief Grid to store simple binary data using a single bit per square.
     * @tparam width_ width of grid in squares - try to keep width and height powers of 2 for performance.
     * @tparam height_ height of grid in squares - try to keep width and height powers of 2 for performance.
     */
    template<coordinate_t width_, coordinate_t height_>
    class BitfieldFullGrid : private BaseGrid<bool> {
        static_assert(width_ > 0 && height_ > 0, "Width and height must be positive");
    public:
        BitfieldFullGrid(){}
        inline bool get(const Point& at) {
            LOGGER_ASSERT_EXCEPT(at.x() >= 0 && at.y() >= 0 && at.x() < width_ && at.y() < height_);
            return (impl[at.x()/8][at.y()] >> (at.x()%8)) & 1;
        }
        inline void set(const Point& at, const bool value) {
            LOGGER_ASSERT_EXCEPT(at.x() >= 0 && at.y() >= 0 && at.x() < width_ && at.y() < height_);
            if (value) impl[at.x()/8][at.y()] |= (1 << (at.x()%8));
            else impl[at.x()/8][at.y()] &= ~(1 << (at.x()%8));
        }
        inline void fill(const bool& value){
            for (int x = 0; x < (width_/8+(width_%8 > 0)); x++)
                for (int y = 0; y < height_; y++)
                    impl[x][y] = value;
        }

        static constexpr coordinate_t width(){ return width_;}
        static constexpr coordinate_t height(){ return height_;}
        typedef bool value_type;
    private:
        char impl[width_/8+(width_%8 > 0)][height_];
    };
}
