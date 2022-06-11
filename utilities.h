//
// Created by Родион Каргаполов on 10.06.2022.
//
#pragma once

#include <string_view>

namespace utilities {
     std::string_view ltrim(std::string_view str);
     std::string_view rtrim(std::string_view str);
     std::string_view trim(std::string_view str);
 }