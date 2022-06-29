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

    inline double ComputeDistance(const Coordinates from, const Coordinates to) {
        using namespace std;
        if (from == to) {
            return 0;
        }
        static const double dr = 3.1415926535 / 180.;
        return acos(sin(from.lat * dr) * sin(to.lat * dr)
                    + cos(from.lat * dr) * cos(to.lat * dr) * cos(abs(from.lng - to.lng) * dr))
               * 6371000;
    }
}

inline std::string_view Ltrim(std::string_view str) {
    const auto pos(str.find_first_not_of(" \t\n\r\f\v"));
    str.remove_prefix(std::min(pos, str.length()));
    return str;
}

inline std::string_view Rtrim(std::string_view str) {
    const auto pos(str.find_last_not_of(" \t\n\r\f\v"));
    str.remove_suffix(std::min(str.length() - pos - 1, str.length()));
    return str;
}

inline std::string_view Trim(std::string_view str) {
    str = Ltrim(str);
    str = Rtrim(str);
    return str;
}