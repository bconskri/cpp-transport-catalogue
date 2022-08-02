#pragma once

#include "request_handler.h"
#include "transport_catalogue.h"
#include "json_builder.h"
#include "route_manager.h"

namespace json_reader {
    //realize StreamData query performerser
    class JsonData : public request_handler::QueryHandler {
    public:
        JsonData() : output_json_builder_(*new json::Builder{}) {};

        //realize loader from stream
        void PerfomUploadQueries(request_handler::Inputer *input) override;

        void PerfomStatQueries(request_handler::Inputer *input,
                               request_handler::Logger *output = nullptr) override;

        void PerfomQueries(request_handler::Inputer *input,
                           request_handler::Logger *output = nullptr) override;

        std::istream &GetStream();

    private:
        json::Builder &output_json_builder_;

        //write data from stream into catalogue
        void ParseStop(json::Node &request);

        void ParseBus(json::Node &request);

        void ParsePerformUploadQueries(std::vector<json::Node> &upload_requests);

        void ParsePerformStatQueries(std::vector<json::Node> &stat_requests);

        void PerformBusQuery(json::Node &request);

        void PerformStopQuery(json::Node &request);

        void ParseRenderSettings(json::Dict &render_settings);

        void ParseRoutingSettings(json::Dict &routing_settings);

        void PerformRouteQuery(json::Node &request);

        void PerformMapQuery();
    };

    template<typename T>
    JsonData &operator>>(JsonData &classObj, T into) {
        classObj.GetStream() >> into;
        return classObj;
    }
} // namespace txt_reader