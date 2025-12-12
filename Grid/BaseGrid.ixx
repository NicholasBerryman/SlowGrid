//
// Created by nickberryman on 9/12/25.
//
export module SG_Grid:BaseGrid;
import :Point;
import SG_GridConfigs;
import Logger;

export namespace SG_Grid {
    /**
     * @brief Interface for grids. Should only be inherited from, never used directly
     * 
     */
    template<typename T>
    class BaseGrid {
    public:
        inline const T& get(const Point& at){Logging::assert_except(0); return nullptr;}
        inline void set(const Point& at, const T& value){Logging::assert_except(0);}
        inline coordinate_t width(){Logging::assert_except(0); return 0;}
        inline coordinate_t height(){Logging::assert_except(0); return 0;}
        inline void fill(const T& value){Logging::assert_except(0);}
        typedef T value_type;

    protected:
        BaseGrid() = default;
    };
}
