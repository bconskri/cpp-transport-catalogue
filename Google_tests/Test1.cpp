//
// Created by Родион Каргаполов on 10.06.2022.
//
#include "gtest/gtest.h"
#include "../transport-catalogue/input_reader.h"
#include "../transport-catalogue/stat_reader.h"
#include "../transport-catalogue/transport_catalogue.h"

#include <sstream>

TEST(Test1, Test1
) {

std::istringstream input{
        "  3  \n"
        " Bus   750 long  : Tolstopaltsevo 2    -   Marushkino  \n"
        "Stop   Tolstopaltsevo 2 : 55.611087 , 37.208290\n"
        "Stop Marushkino: 55.595884, 37.209755\n"
};

TransportCatalogue transport_catalogue{};
//perform upload queries
auto upload_query_handler = input_reader::QueryHandler::GetHandler(Console,
                                                                   input);
upload_query_handler->
PerfomUploadQueries(transport_catalogue);

std::istringstream stat_input{
        " 2 \n"
        "  Bus    750 long  \n"
        "  Bus    Gazel new  \n"
};

//perform stat queries
auto stat_query_handler = stat_reader::QueryHandler::GetHandler(Console,
                                                                stat_input);
stat_query_handler->
PerfomStatQueries(transport_catalogue);
}