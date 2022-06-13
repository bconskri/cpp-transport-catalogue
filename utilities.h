//
// Created by Родион Каргаполов on 10.06.2022.
//
#pragma once

#include <string_view>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>

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

inline std::vector<std::string> SplitByChar (const std::string &s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss (s);
    std::string item;

    while (getline (ss, item, delim)) {
        result.push_back(std::string(Trim(item)));
    }

    return result;
}

//inline void AssertImpl(bool value, const std::string &expr_str, const std::string &file, const std::string &func, unsigned line,
//                const std::string &hint) {
//    using namespace std::literals;
//
//    if (!value) {
//        std::cerr << file << "("s << line << "): "s << func << ": "s;
//        std::cerr << "Assert("s << expr_str << ") failed."s;
//        if (!hint.empty()) {
//            std::cerr << " Hint: "s << hint;
//        }
//        std::cerr << std::endl;
//        std::abort();
//    }
//}
//
//#define ASSERT_HINT(expr, hint) AssertImpl(!!(expr), #expr, __FILE__, __FUNCTION__, __LINE__, (hint))