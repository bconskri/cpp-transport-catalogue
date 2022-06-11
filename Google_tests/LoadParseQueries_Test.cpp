//
// Created by Родион Каргаполов on 10.06.2022.
//
#include "gtest/gtest.h"
#include "../input_reader.h"
#include "../transport_catalogue.h"

TEST(LoadParseQuery, StreamLoadParseQuery){
    transport_catalogue::TransportCatalogue transport_catalogue{};
    auto QueryLoader = input_reader::QueryPerformer::GetLoader(input_reader::input_type::Dstream);
    QueryLoader->LoadPerfomQueries(transport_catalogue);
    QueryLoader->LoadPerfomQueries(transport_catalogue);
}