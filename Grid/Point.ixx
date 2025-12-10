//
// Created by nickberryman on 9/12/25.
//
module;
#include <cstdlib>
#include <cmath>

export module SG_Grid:Point;
import SG_GridConfigs;
import Logger;

SG_Grid::float_distance_t roundToNearestMultiple(const SG_Grid::float_distance_t number, const SG_Grid::float_distance_t multiple) { return std::round(number/multiple) * multiple; }

export namespace SG_Grid {
    /**
     * @brief Struct for coordinates
     * 
     */
    class Point {
    public:
        inline Point(const coordinate_t& x, const coordinate_t& y) : _x(x), _y(y) {}
        [[nodiscard]] inline const coordinate_t& x() const {return _x;}
        [[nodiscard]] inline const coordinate_t& y() const {return _y;}
        inline void setX(coordinate_t& x){_x = x;}
        inline void setY(coordinate_t& y){_y = y;}

        inline Point operator+(const Point& other) const { return Point(_x + other._x, _y + other._y); }
        inline Point operator-(const Point& other) const { return Point(_x - other._x, _y - other._y); }
        inline Point operator*(const Point& other) const { return Point(_x * other._x, _y * other._y); }
        inline Point operator/(const Point& other) const { return Point(_x / other._x, _y / other._y); }
        inline Point operator%(const Point& other) const { return Point(_x % other._x, _y % other._y); }

        inline Point& operator+=(const Point& other) {_x += other._x; _y += other._y; return *this; }
        inline Point& operator-=(const Point& other) {_x -= other._x; _y -= other._y; return *this; }
        inline Point& operator*=(const Point& other) {_x *= other._x; _y *= other._y; return *this; }
        inline Point& operator/=(const Point& other) {_x /= other._x; _y /= other._y; return *this; }
        inline Point& operator%=(const Point& other) {_x %= other._x; _y %= other._y; return *this; }

        inline bool operator==(const Point& rhs) const {return _x == rhs._x && _y == rhs._y;}
        inline bool operator!=(const Point& rhs) const {return _x != rhs._x || _y != rhs._y;}
        inline Point operator>=(const Point& rhs) const {return Point(_x >= rhs._x, _y >= rhs._y);}
        inline Point operator<=(const Point& rhs) const {return Point(_x <= rhs._x, _y <= rhs._y);}
        inline Point operator>(const Point& rhs) const {return Point(_x > rhs._x, _y > rhs._y);}
        inline Point operator<(const Point& rhs) const {return Point(_x < rhs._x, _y < rhs._y);}

        inline Point& abs() {_x = std::abs(_x); _y = std::abs(_y); return *this;}

    private:
        coordinate_t _x;
        coordinate_t _y;
    };

    namespace Distance {
        coordinate_t Chebyshev(const Point& a, const Point& b) { return std::max(std::abs(a.x() - b.x()), std::abs(a.y() - b.y())); }
        coordinate_t Manhattan(const Point& a, const Point& b) { return std::abs(a.x() - b.x()) + std::abs(a.y() - b.y()); }
        coordinate_t ComparativeEuclidean(const Point& a, const Point& b) { return std::pow(std::abs(a.x() - b.x()),2) + std::pow(std::abs(a.y() - b.y()),2); }
        float_distance_t AbsoluteEuclidean(const Point& a, const Point& b) {return std::sqrt(std::pow(std::abs(a.x() - b.x()),2) + std::pow(std::abs(a.y() - b.y()),2));}
    }

    namespace Bearing {
        float_distance_t TrueBearing(const Point& from, const Point& to){return atan2(to.x()-from.x(), to.y()-from.y()); }
        float_distance_t QueensBearing(const Point& from, const Point& to){ return roundToNearestMultiple(TrueBearing(from, to),0.25*M_PI);}
        float_distance_t RooksBearing(const Point& from, const Point& to){ return roundToNearestMultiple(TrueBearing(from, to),0.5*M_PI);}

        coordinate_t rad2deg(const float_distance_t rad) { return static_cast<SG_Grid::coordinate_t>(rad * 180.0 / M_PI); }
        float_distance_t deg2rad(const coordinate_t deg) { return deg * M_PI / 180; }
    }
}
