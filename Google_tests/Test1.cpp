//
// Created by Родион Каргаполов on 10.06.2022.
//
#include "gtest/gtest.h"
#include "../input_reader.h"
#include "../stat_reader.h"
#include "../transport_catalogue.h"

#include <sstream>

TEST(Test1, Test1
) {

std::istringstream input{
        "3\n"
        "Stop Tolstopaltsevo: 55.611087, 37.208290\n"
        "Stop Marushkino: 55.595884, 37.209755\n"
        "Bus 750: Tolstopaltsevo - Marushkino\n"
};

TransportCatalogue transport_catalogue{};
//perform upload queries
auto upload_query_handler = input_reader::QueryHandler::GetHandler(Dstream,
                                                                   input);
upload_query_handler->
PerfomUploadQueries(transport_catalogue);

std::istringstream stat_input{
        "1\n"
        "Bus 750\n"
};

//perform stat queries
auto stat_query_handler = stat_reader::QueryHandler::GetHandler(Dstream,
                                                                stat_input);
stat_query_handler->
PerfomStatQueries(transport_catalogue);
}