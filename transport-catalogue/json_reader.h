#pragma once

#include "request_handler.h"
#include "transport_catalogue.h"
#include "json.h"

namespace json_reader {
    //realize StreamData query performerser
    class JsonData : public request_handler::QueryHandler {
    public:
        //realize loader from stream
    public:
        //realize loader from stream
        void PerfomUploadQueries(TransportCatalogue &transport_catalogue, request_handler::Inputer *input) override;

        void PerfomStatQueries(TransportCatalogue &transport_catalogue, request_handler::Inputer *input,
                               request_handler::Logger *output = nullptr) override;

        void PerfomQueries(TransportCatalogue &transport_catalogue, request_handler::Inputer *input,
                           request_handler::Logger *output = nullptr) override;

        std::istream &GetStream();

    private:
        //write data from stream into catalogue
        void parse_perform_upload_queries(const std::vector<json::Node> &upload_requests) const;

        void parse_perform_stat_queries(TransportCatalogue &transport_catalogue,
                                        const int q_count, request_handler::Inputer *input,
                                        request_handler::Logger *output = nullptr);

        void OutputBusInfo(TransportCatalogue &transport_catalogue,
                           const std::string_view busname_to_output, request_handler::Logger *output) const;

        void OutputStopInfo(TransportCatalogue &transport_catalogue,
                            const std::string_view stopname_to_output, request_handler::Logger *output) const;
    };

    template<typename T>
    JsonData &operator>>(JsonData &classObj, T into) {
        classObj.GetStream() >> into;
        return classObj;
    }
} // namespace txt_reader