//#include "gtest/gtest.h"
#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"
#include "utilities.h"

#include <sstream>
#include <fstream>

int main() {
    std::istringstream input{
            "10\n"
            "Stop Tolstopaltsevo: 55.611087, 37.208290\n"
            "Stop Marushkino: 55.595884, 37.209755\n"
            "Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye\n"
            "Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka\n"
            "Stop Rasskazovka: 55.632761, 37.333324\n"
            "Stop Biryulyovo Zapadnoye: 55.574371, 37.651700\n"
            "Stop Biryusinka: 55.581065, 37.648390\n"
            "Stop Universam: 55.587655, 37.645687\n"
            "Stop Biryulyovo Tovarnaya: 55.592028, 37.653656\n"
            "Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164\n"
    };

    TransportCatalogue transport_catalogue{};
    //perform upload queries
    auto upload_query_handler = input_reader::QueryHandler::GetHandler(Console,
                                                                       input);
    upload_query_handler->PerfomUploadQueries(transport_catalogue);

    std::istringstream stat_input{
            "3\n"
            "Bus 256\n"
            "Bus 750\n"
            "Bus 751\n"
    };

    //perform stat queries
    auto stat_query_handler = stat_reader::QueryHandler::GetHandler(Console,
                                                                    stat_input);
    stat_query_handler->PerfomStatQueries(transport_catalogue);

    return 0;
}