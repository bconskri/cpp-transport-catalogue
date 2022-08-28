#include "transport-catalogue/request_handler.h"
#include "transport-catalogue/transport_catalogue.h"

#include <fstream>
#include <iostream>
#include <string_view>

using namespace std::literals;

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);

    if (mode == "make_base"sv) {

        // make base here
        auto in = request_handler::Inputer::GetInputer(request_handler::io_stream::Console);

        //perform upload queries & stat queries in one
        auto upload_query_handler = request_handler::QueryHandler::GetHandler(request_handler::io_type::Json);
        upload_query_handler->PerfomUploadQueries(in);

    } else if (mode == "process_requests"sv) {

        // process requests here
        auto in = request_handler::Inputer::GetInputer(request_handler::io_stream::Console);
        auto out = request_handler::Logger::GetLogger(request_handler::io_stream::Console);

        //perform upload queries & stat queries in one
        auto upload_query_handler = request_handler::QueryHandler::GetHandler(request_handler::io_type::Json);
        upload_query_handler->PerfomStatQueries(in, out);

    } else {
        PrintUsage();
        return 1;
    }
}