#pragma once

#include "transport_catalogue.h"
#include <iostream>

namespace input_reader {
    using namespace transport_catalogue;

    enum input_type { Dstream=0, Json, Xml };

    //fabric of query performes
    class QueryPerformer {
    public:
        virtual ~QueryPerformer() = default;
        //make load queries from data search and perform write or read data to/from transport_catalogue
        virtual void LoadPerfomQueries(const TransportCatalogue &transport_catalogue) = 0;
        //create loader from specified data search
        static QueryPerformer* GetLoader(const input_type datasearch, std::istream& input);
    };

    //realize StreamData query performerser
    class StreamData : public QueryPerformer
    {
    public:
        StreamData(std::istream& input = std::cin)
            : input_(input)
            , output_(std::cout) {}
        //realize loader from stream
        void LoadPerfomQueries(const TransportCatalogue &transport_catalogue) {};

    private:
        std::istream& input_;
        std::ostream& output_;
        //write data from stream into catalogue
        void perform_write_queries(const TransportCatalogue &transport_catalogue,
                                   const int n,
                                   std::istream& input);
        //read queries from stream and from catalogue and write report to output stream
        //void perform_read_queries(const int n, std::istream& input, std::ostream *output);
    };

    QueryPerformer *QueryPerformer::GetLoader(const input_type datasearch, std::istream& input = std::cin) {
        QueryPerformer *p;
        switch (datasearch) {
            case Dstream:
                p = new StreamData(input);
                break;

            default:
                assert(false);
        }
        return p;
    };
}