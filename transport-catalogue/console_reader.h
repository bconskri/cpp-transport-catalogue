#pragma once

#include "transport_catalogue.h"
#include "geo.h"
#include "request_handler.h"

#include <iostream>
#include <algorithm>
#include <cassert>
#include <queue>

namespace console_reader {
    //realize StreamData query performerser
    class StreamData : public request_handler::QueryHandler {
    public:
        StreamData(std::istream &input = std::cin)
                : input_(input) {}

        //realize loader from stream
        void PerfomUploadQueries(TransportCatalogue &transport_catalogue);
        void PerfomStatQueries(TransportCatalogue &transport_catalogue, request_handler::Logger *output = nullptr);

    private:
        std::istream &input_;

        //write data from stream into catalogue
        void parse_perform_stat_queries(TransportCatalogue &transport_catalogue,
                                        const int n, request_handler::Logger *output = nullptr);

        void OutputBusInfo(TransportCatalogue &transport_catalogue,
                           const std::string_view busname_to_output, request_handler::Logger *output) const;

        void OutputStopInfo(TransportCatalogue &transport_catalogue,
                            const std::string_view stopname_to_output, request_handler::Logger *output) const;
    };
}