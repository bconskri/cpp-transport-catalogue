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
        json::Node output_json_root_;

        //write data from stream into catalogue
        void ParseStop(json::Node &request);

        void ParseBus(json::Node &request);

        void ParsePerformUploadQueries(std::vector<json::Node> &upload_requests);

        void ParsePerformStatQueries(std::vector<json::Node> &stat_requests);

        void PerformBusQuery(json::Node &request, json::Dict &response);

        void PerformStopQuery(json::Node &request, json::Dict &response);

        void ParseRenderSettings(json::Dict &render_settings);

        void PerformMapQuery(json::Dict &response);
    };

    template<typename T>
    JsonData &operator>>(JsonData &classObj, T into) {
        classObj.GetStream() >> into;
        return classObj;
    }
} // namespace txt_reader