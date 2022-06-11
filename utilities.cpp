//
// Created by Родион Каргаполов on 10.06.2022.
//

#include "utilities.h"

namespace utilities {
    std::string_view ltrim(std::string_view str) {
        const auto pos(str.find_first_not_of(" \t\n\r\f\v"));
        str.remove_prefix(std::min(pos, str.length()));
        return str;
    }

    std::string_view rtrim(std::string_view str) {
        const auto pos(str.find_last_not_of(" \t\n\r\f\v"));
        str.remove_suffix(std::min(str.length() - pos - 1, str.length()));
        return str;
    }

    std::string_view trim(std::string_view str) {
        str = ltrim(str);
        str = rtrim(str);
        return str;
    }
}