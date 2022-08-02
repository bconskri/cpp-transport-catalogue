cmake_minimum_required(VERSION 3.22)
project(TransportCatalogue)

set(CMAKE_CXX_STANDARD 17)

#recent -Wall -pedantic -Wextra -Wstrict-overflow -Werror=vla
#for sanitizer  -fsanitize=address -g -O0 -fno-omit-frame-pointer -fno-optimize-sibling-calls
#for profiling -O0 -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -Wstrict-overflow -Werror=vla -fsanitize=address -g -O0 -fno-omit-frame-pointer -fno-optimize-sibling-calls")

add_executable(TransportCatalogue main.cpp transport-catalogue/geo.h transport-catalogue/txt_reader.cpp transport-catalogue/txt_reader.h transport-catalogue/transport_catalogue.cpp transport-catalogue/transport_catalogue.h transport-catalogue/json.h transport-catalogue/json.cpp transport-catalogue/json_reader.cpp transport-catalogue/json_reader.h transport-catalogue/domain.h transport-catalogue/request_handler.cpp transport-catalogue/request_handler.h transport-catalogue/geo.cpp transport-catalogue/svg.h transport-catalogue/svg.cpp transport-catalogue/map_renderer.cpp transport-catalogue/map_renderer.h transport-catalogue/domain.cpp transport-catalogue/json_builder.h transport-catalogue/json_builder.cpp transport-catalogue/router.h transport-catalogue/graph.h transport-catalogue/ranges.h transport-catalogue/transport_router.cpp transport-catalogue/transport_router.h)

#add_subdirectory(Google_tests)