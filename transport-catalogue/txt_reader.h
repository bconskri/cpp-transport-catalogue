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
        void PerfomUploadQueries(request_handler::Inputer *input) override;

        void PerfomStatQueries(request_handler::Inputer *input,
                               request_handler::Logger *output = nullptr) override;

        void PerfomQueries(request_handler::Inputer *input,
                           request_handler::Logger *output = nullptr) override; //fixme;

    private:
        //write data from stream into catalogue
        void ParsePerformStatQueries(const int q_count, request_handler::Inputer *input,
                                     request_handler::Logger *output = nullptr);

        void OutputBusInfo(const std::string_view busname_to_output, request_handler::Logger *output) const;

        void OutputStopInfo(const std::string_view stopname_to_output, request_handler::Logger *output) const;
    };
} // namespace txt_reader