#pragma once

#include <cmath>
#include <iomanip>
#include <iostream>

namespace geo {
    struct Coordinates {
        double lat;
        double lng;

        bool operator==(const Coordinates &other) const {
            return lat == other.lat && lng == other.lng;
        }

        bool operator!=(const Coordinates &other) const {
            return !(*this == other);
        }
    };

    double ComputeDistance(const Coordinates from, const Coordinates to);
}//namespace geo