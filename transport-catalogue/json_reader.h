#pragma once

#include "request_handler.h"
#include "transport_catalogue.h"
#include "json.h"

namespace json_reader {
    //realize StreamData query performerser
    class JsonData : public request_handler::QueryHandler {
    public:
        //realize loader from stream
        void PerfomUploadQueries(request_handler::Inputer *input) override;

        void PerfomStatQueries(request_handler::Inputer *input,
                               request_handler::Logger *output = nullptr) override;

        void PerfomQueries(request_handler::Inputer *input,
                           request_handler::Logger *output = nullptr) override;

        std::istream &GetStream();

    private:
        json::Node root_;

        //write data from stream into catalogue
        void parse_stop(json::Node &request);

        void parse_bus(json::Node &request);

        void parse_perform_upload_queries(std::vector<json::Node> &upload_requests);

        void parse_perform_stat_queries(std::vector<json::Node> &stat_requests);

        void perform_bus_query(json::Node &request, json::Dict &response);

        void perform_stop_query(json::Node &request, json::Dict &response);
    };

    template<typename T>
    JsonData &operator>>(JsonData &classObj, T into) {
        classObj.GetStream() >> into;
        return classObj;
    }
} // namespace txt_reader