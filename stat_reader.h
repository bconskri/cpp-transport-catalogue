#pragma once

#include "transport_catalogue.h"
#include "utilities.h"

#include <iostream>
#include <algorithm>
#include <cassert>

namespace stat_reader {

//fabric of query performes
    class QueryHandler {
    public:
        virtual ~QueryHandler() = default;

        //make load queries from data search and perform write or read data to/from transport_catalogue
        virtual void PerfomStatQueries(TransportCatalogue &transport_catalogue) = 0;

        //create loader from specified data search
        static QueryHandler *GetHandler(const io_type datasearch, std::istream &input);
    };

//realize StreamData query performerser
    class StreamData : public QueryHandler {
    public:
        StreamData(std::istream &input = std::cin)
                : input_(input), output_(std::cout) {}

        //realize loader from stream
        void PerfomStatQueries(TransportCatalogue &transport_catalogue);

    private:
        std::istream &input_;
        [[maybe_unused]] std::ostream &output_;

        //write data from stream into catalogue
        void parse_perform_stat_queries(TransportCatalogue &transport_catalogue,
                                          const int n);
    };
}