//
// Created by nickberryman on 9/12/25.
//
#include <cstdint>

#include "Logger.h"
#include <iostream>
#include <cmath>
#define assert Logging::assert_except

import Logger;
import SG_Grid;

void testOnStack(){
    SG_Grid::Point pt1(0,1);
    SG_Grid::Point pt2(3,3);

    assert(pt1.x() == 0);
    assert(pt1.y() == 1);
    assert(pt2.x() == 3 && pt2.y() == 3);

    SG_Grid::Point pt3(0,0);
    pt3 = pt1 + pt2;
    assert(pt3.x() == pt1.x() + pt2.x() && pt3.y() == pt1.y() + pt2.y());
    pt3 = pt1 * pt2;
    assert(pt3.x() == pt1.x() * pt2.x() && pt3.y() == pt1.y() * pt2.y());
    pt3 = pt2 - pt1;
    assert(pt3.x() == pt2.x() - pt1.x() && pt3.y() == pt2.y() - pt1.y());
    pt3 = pt1 / pt2;
    assert(pt3.x() == pt1.x() / pt2.x() && pt3.y() == pt1.y() / pt2.y());

    pt3 -= pt3;
    pt3 += pt1;
    assert(pt3 == pt1);
    assert(pt3 != pt2);

    assert((pt2 > pt1).x());

    assert(SG_Grid::Distance::Manhattan(pt1, pt2) == 5);
    assert(SG_Grid::Distance::Chebyshev(pt2, pt1) == 3);
    assert(SG_Grid::Distance::ComparativeEuclidean(pt1, pt2) == 13);
    assert(SG_Grid::Distance::AbsoluteEuclidean(pt1, pt2) > 3.5 && SG_Grid::Distance::AbsoluteEuclidean(pt1, pt2) < 3.7);

    auto bearing = SG_Grid::Bearing::TrueBearing(pt1, pt2);
    assert(bearing > 0.9 && bearing < 1.0);
    assert(SG_Grid::Bearing::rad2deg(bearing) >= 56 && SG_Grid::Bearing::rad2deg(bearing) <= 57);
    assert(SG_Grid::Bearing::deg2rad(SG_Grid::Bearing::rad2deg(bearing)) > 0.85 && SG_Grid::Bearing::deg2rad(SG_Grid::Bearing::rad2deg(bearing)) < 1.05);
    assert( std::fabs(SG_Grid::Bearing::QueensBearing(pt1, pt2) - 0.25*M_PI) < 0.1);
    assert( std::fabs(SG_Grid::Bearing::RooksBearing(pt1, pt2) - 0.5*M_PI) < 0.1);
}

int main(int, char**) {
    testOnStack();
    return 0;
}

#undef assert

