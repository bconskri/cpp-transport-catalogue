cmake_minimum_required(VERSION 3.22)
project(TransportCatalogue)

set(CMAKE_CXX_STANDARD 17)

find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS transport-catalogue/transport_catalogue.proto)

#add_subdirectory(Google_tests)

#recent -Wall -pedantic -Wextra -Wstrict-overflow -Werror=vla
#for sanitizer  -fsanitize=address -g -O0 -fno-omit-frame-pointer -fno-optimize-sibling-calls
#for profiling -O0 -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -Wstrict-overflow -Werror=vla -fsanitize=address -g -O0 -fno-omit-frame-pointer -fno-optimize-sibling-calls")

set(TRANSPORT_CATALOGUE_FILES transport-catalogue/geo.h transport-catalogue/txt_reader.cpp transport-catalogue/txt_reader.h transport-catalogue/transport_catalogue.cpp transport-catalogue/transport_catalogue.h transport-catalogue/json.h transport-catalogue/json.cpp transport-catalogue/json_reader.cpp transport-catalogue/json_reader.h transport-catalogue/domain.h transport-catalogue/request_handler.cpp transport-catalogue/request_handler.h transport-catalogue/geo.cpp transport-catalogue/svg.h transport-catalogue/svg.cpp transport-catalogue/map_renderer.cpp transport-catalogue/map_renderer.h transport-catalogue/domain.cpp transport-catalogue/json_builder.h transport-catalogue/json_builder.cpp transport-catalogue/router.h transport-catalogue/graph.h transport-catalogue/ranges.h transport-catalogue/transport_router.cpp transport-catalogue/transport_router.h transport-catalogue/serialization.h transport-catalogue/serialization.cpp)

add_executable(TransportCatalogue main.cpp ${PROTO_SRCS} ${PROTO_HDRS} ${TRANSPORT_CATALOGUE_FILES})

target_include_directories(TransportCatalogue PUBLIC ${Protobuf_INCLUDE_DIRS})
target_include_directories(TransportCatalogue PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

string(REPLACE "protobuf.lib" "protobufd.lib" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")
string(REPLACE "protobuf.a" "protobufd.a" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")

target_link_libraries(TransportCatalogue "$<IF:$<CONFIG:Debug>,${Protobuf_LIBRARY_DEBUG},${Protobuf_LIBRARY}>" Threads::Threads)