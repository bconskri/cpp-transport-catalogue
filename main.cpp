//#include "gtest/gtest.h"
#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"
#include "utilities.h"

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

    return 0;
}