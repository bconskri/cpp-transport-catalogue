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
}

enum io_type {
    Console = 0, Txt, Json, Xml
};

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