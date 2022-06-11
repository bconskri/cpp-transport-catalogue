cmake_minimum_required(VERSION 3.22)
project(TransportCatalogue)

set(CMAKE_CXX_STANDARD 17)

#recent -Wall -pedantic -Wextra -Wstrict-overflow -Werror=vla
#for sanitizer  -fsanitize=address -g -O1 -fno-omit-frame-pointer -fno-optimize-sibling-calls
#for profiling -O0 -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -Wstrict-overflow -Werror=vla -fsanitize=address -g -O1 -fno-omit-frame-pointer -fno-optimize-sibling-calls")

add_executable(TransportCatalogue main.cpp geo.h input_reader.cpp input_reader.h stat_reader.cpp stat_reader.h transport_catalogue.cpp transport_catalogue.h utilities.h)

add_subdirectory(Google_tests)