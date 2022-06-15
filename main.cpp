//#include "gtest/gtest.h"
#include "transport-catalogue/input_reader.h"
#include "transport-catalogue/stat_reader.h"
#include "transport-catalogue/transport_catalogue.h"
#include "transport-catalogue/geo.h"

#include <sstream>
#include <fstream>

int main() {

    TransportCatalogue transport_catalogue{};
    //perform upload queries
    auto upload_query_handler = input_reader::QueryHandler::GetHandler(Console);
    upload_query_handler->PerfomUploadQueries(transport_catalogue);

    //perform stat queries
    auto stat_query_handler = stat_reader::QueryHandler::GetHandler(Console);
    stat_query_handler->PerfomStatQueries(transport_catalogue);

//    std::stringstream input;
//    std::ifstream file("../pt1/tsC_case1_input.txt");
//
//    if (file) {
//        input << file.rdbuf();
//        file.close();
//
//        std::string line;
//
//        TransportCatalogue transport_catalogue{};
//        //perform upload queries
//        auto upload_query_handler = input_reader::QueryHandler::GetHandler(Console,
//                                                                           input);
//        upload_query_handler->PerfomUploadQueries(transport_catalogue);
//
//        std::cerr << "Uploading finish" << std::endl;
//
//        //perform stat queries
//        auto stat_query_handler = stat_reader::QueryHandler::GetHandler(Console,
//                                                                        input);
//        auto file_output = stat_reader::FileLogger("tsC_case2_output.txt");
//        stat_query_handler->PerfomStatQueries(transport_catalogue, &file_output);
//    }

    return 0;
}