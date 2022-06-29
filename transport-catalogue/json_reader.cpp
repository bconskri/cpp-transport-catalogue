#include "json_reader.h"

#include "request_handler.h"
#include "transport_catalogue.h"
#include "json.h"

namespace json_reader {

    void JsonData::PerfomUploadQueries(TransportCatalogue &transport_catalogue, request_handler::Inputer *input) {}

    void JsonData::PerfomStatQueries(TransportCatalogue &transport_catalogue, request_handler::Inputer *input,
                                     request_handler::Logger *output){}

    void JsonData::PerfomQueries(TransportCatalogue &transport_catalogue, request_handler::Inputer *input,
                                 request_handler::Logger *output) {}

    void JsonData::parse_perform_stat_queries(TransportCatalogue &transport_catalogue,
                                              const int q_count, request_handler::Inputer *input,
                                              request_handler::Logger *output) {}

    void JsonData::OutputBusInfo(TransportCatalogue &transport_catalogue,
                                 const std::string_view busname_to_output, request_handler::Logger *output) const {}

    void JsonData::OutputStopInfo(TransportCatalogue &transport_catalogue,
                                  const std::string_view stopname_to_output, request_handler::Logger *output) const {}
} // namespace txt_reader