cmake_minimum_required(VERSION 3.19)
project(transport_catalogue)

set(CMAKE_CXX_STANDARD 17)

find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS transport-catalogue/transport_catalogue.proto)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -Wstrict-overflow -Werror=vla -fsanitize=address -g -O0 -fno-omit-frame-pointer -fno-optimize-sibling-calls")

set(TRANSPORT_CATALOGUE_FILES geo.h txt_reader.cpp txt_reader.h transport_catalogue.cpp transport_catalogue.h json.h json.cpp json_reader.cpp json_reader.h domain.h request_handler.cpp request_handler.h geo.cpp svg.h svg.cpp map_renderer.cpp map_renderer.h domain.cpp json_builder.h json_builder.cpp router.h graph.h ranges.h transport_router.cpp transport_router.h serialization.h serialization.cpp)
list(TRANSFORM TRANSPORT_CATALOGUE_FILES PREPEND "transport-catalogue/")

add_executable(transport_catalogue main.cpp ${PROTO_SRCS} ${PROTO_HDRS} ${TRANSPORT_CATALOGUE_FILES})

target_include_directories(transport_catalogue PUBLIC ${Protobuf_INCLUDE_DIRS})
target_include_directories(transport_catalogue PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

string(REPLACE "protobuf.lib" "protobufd.lib" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")
string(REPLACE "protobuf.a" "protobufd.a" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")

target_link_libraries(transport_catalogue "$<IF:$<CONFIG:Debug>,${Protobuf_LIBRARY_DEBUG},${Protobuf_LIBRARY}>" Threads::Threads)