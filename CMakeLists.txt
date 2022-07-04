cmake_minimum_required(VERSION 3.22)
project(TransportCatalogue)

set(CMAKE_CXX_STANDARD 17)

#recent -Wall -pedantic -Wextra -Wstrict-overflow -Werror=vla
#for sanitizer  -fsanitize=address -g -O0 -fno-omit-frame-pointer -fno-optimize-sibling-calls
#for profiling -O0 -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -Wstrict-overflow -Werror=vla -fsanitize=address -g -O0 -fno-omit-frame-pointer -fno-optimize-sibling-calls")

add_executable(TransportCatalogue main.cpp transport-catalogue/geo.h transport-catalogue/input_reader.cpp transport-catalogue/input_reader.h transport-catalogue/stat_reader.cpp transport-catalogue/stat_reader.h transport-catalogue/transport_catalogue.cpp transport-catalogue/transport_catalogue.h transport-catalogue/geo.cpp)

#add_subdirectory(Google_tests)