#include "json_reader.h"

#include "request_handler.h"
#include "transport_catalogue.h"
#include "json.h"

namespace json_reader {
    void JsonData::parse_perform_upload_queries(const std::vector<json::Node> &upload_requests) const {
        for (const auto &request: upload_requests) {
            if (request.AsMap().at("type").AsString() == "Stop") {
                //parse_stop(request);
            }

            if (request.AsMap().at("type").AsString() == "Bus") {
                //parse_bus(request);
            }
        }
    }

    void JsonData::PerfomUploadQueries(TransportCatalogue &transport_catalogue, request_handler::Inputer *input) {
        auto root = json::Load(input->GetStream()).GetRoot();
        parse_perform_upload_queries(root.AsMap().at("base_requests").AsArray());
    }

    void JsonData::PerfomStatQueries(TransportCatalogue &transport_catalogue, request_handler::Inputer *input,
                                     request_handler::Logger *output) {
        auto root = json::Load(input->GetStream()).GetRoot();
        parse_perform_stat_queries(root.AsMap().at("stat_requests").AsArray(), output);
    }

    void JsonData::PerfomQueries(TransportCatalogue &transport_catalogue, request_handler::Inputer *input,
                                 request_handler::Logger *output) {
        auto root = json::Load(input->GetStream()).GetRoot();
        parse_perform_stat_queries(root.AsMap().at("base_requests").AsArray());
        parse_perform_upload_queries(root.AsMap().at("stat_requests").AsArray(), output);
    }

    void JsonData::parse_perform_stat_queries(TransportCatalogue &transport_catalogue,
                                              request_handler::Inputer *input,
                                              request_handler::Logger *output) {}

    void JsonData::OutputBusInfo(TransportCatalogue &transport_catalogue,
                                 const std::string_view busname_to_output, request_handler::Logger *output) const {}

    void JsonData::OutputStopInfo(TransportCatalogue &transport_catalogue,
                                  const std::string_view stopname_to_output, request_handler::Logger *output) const {}
} // namespace txt_reader