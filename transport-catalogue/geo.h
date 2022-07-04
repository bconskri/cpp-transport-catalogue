#pragma once

#include <cmath>
#include <iomanip>
#include <iostream>

namespace geo {
    struct Coordinates {
        double lat;
        double lng;

        bool operator==(const Coordinates &other) const;

        bool operator!=(const Coordinates &other) const;
    };

    double ComputeDistance(const Coordinates from, const Coordinates to);
}//namespace geo