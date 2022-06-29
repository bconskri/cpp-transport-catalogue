#pragma once

#include "transport_catalogue.h"
#include "geo.h"
#include "request_handler.h"

#include <iostream>
#include <algorithm>
#include <cassert>
#include <queue>

namespace txt_reader {
    //realize StreamData query performerser
    class TxtData : public request_handler::QueryHandler {
    public:
        //realize loader from stream
        void PerfomUploadQueries(TransportCatalogue &transport_catalogue, request_handler::Inputer *input) override;

        void PerfomStatQueries(TransportCatalogue &transport_catalogue, request_handler::Inputer *input,
                               request_handler::Logger *output = nullptr) override;

        void PerfomQueries(TransportCatalogue &transport_catalogue, request_handler::Inputer *input,
                           request_handler::Logger *output = nullptr) override; //fixme;
        std::istream &GetStream();

    private:
        //write data from stream into catalogue
        void parse_perform_stat_queries(TransportCatalogue &transport_catalogue,
                                        const int q_count, request_handler::Inputer *input,
                                        request_handler::Logger *output = nullptr);

        void OutputBusInfo(TransportCatalogue &transport_catalogue,
                           const std::string_view busname_to_output, request_handler::Logger *output) const;

        void OutputStopInfo(TransportCatalogue &transport_catalogue,
                            const std::string_view stopname_to_output, request_handler::Logger *output) const;
    };

    template<typename T>
    TxtData &operator>>(TxtData &classObj, T into) {
        classObj.GetStream() >> into;
        return classObj;
    }
} // namespace txt_reader