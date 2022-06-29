//#include "gtest/gtest.h"
#include "transport-catalogue/request_handler.h"
#include "transport-catalogue/transport_catalogue.h"

#include <sstream>
#include <fstream>

int main() {
    using namespace request_handler;
    using namespace std::literals;

    TransportCatalogue transport_catalogue{};
    auto in = Inputer::GetInputer(io_stream::Console);
    auto out = Logger::GetLogger(io_stream::Console);
    auto in_f = Inputer::GetInputer(io_stream::File, ""s);
    auto out_f = Logger::GetLogger(io_stream::File, ""s);

    //perform upload queries
    auto upload_query_handler = request_handler::QueryHandler::GetHandler(io_type::Txt);
    upload_query_handler->PerfomUploadQueries(transport_catalogue, in);

    //perform stat queries
    auto stat_query_handler = request_handler::QueryHandler::GetHandler(io_type::Txt);
    stat_query_handler->PerfomStatQueries(transport_catalogue, in, out);

    //чтение формата TXT источник Console
//    TransportCatalogue transport_catalogue{};
//    //perform upload queries
//    auto upload_query_handler = request_handler::QueryHandler::GetHandler(request_handler::Console);
//    upload_query_handler->PerfomUploadQueries(transport_catalogue);
//
//    //perform stat queries
//    auto stat_query_handler = request_handler::QueryHandler::GetHandler(request_handler::Console);
//    stat_query_handler->PerfomStatQueries(transport_catalogue);

// ----------------------------------
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