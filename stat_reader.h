#pragma once

#include "transport_catalogue.h"
#include "geo.h"

#include <iostream>
#include <algorithm>
#include <cassert>
#include <fstream>

namespace stat_reader {

    class Logger {
    public:
        virtual void log(const std::string_view &) = 0;

        static Logger *GetLogger(const io_type datasearch, std::string file_name = "");
    };

    class ConsoleLogger : public Logger {
    public:
        void log(const std::string_view &msg) override {
            std::cout << msg;
        };
    };

    class FileLogger : public Logger {
    public:
        FileLogger(const std::string &filename) {
            //std::ofstream file(filename);
            //ofs. = &file;
            ofs.open(filename);
        }

        ~FileLogger() {
            if (ofs) {
                ofs.close();
            }
        }

        void log(const std::string_view &msg) override {
            ofs << msg;
        }

    private:
        std::ofstream ofs;
    };

//fabric of query performes
    class QueryHandler {
    public:
        virtual ~QueryHandler() = default;

        //make load queries from data search and perform write or read data to/from transport_catalogue
        virtual void PerfomStatQueries(TransportCatalogue &transport_catalogue, Logger *output = nullptr) = 0;

        //create loader from specified data search
        static QueryHandler *GetHandler(const io_type datasearch, std::istream &input = std::cin);
    };

//realize StreamData query performerser
    class StreamData : public QueryHandler {
    public:
        StreamData(std::istream &input = std::cin)
                : input_(input) {}


        //realize loader from stream
        void PerfomStatQueries(TransportCatalogue &transport_catalogue, Logger *output = nullptr);

    private:
        std::istream &input_;

        //write data from stream into catalogue
        void OutputBusInfo(TransportCatalogue &transport_catalogue,
                           const std::string_view busname_to_output, Logger *output) const;

        void OutputStopInfo(TransportCatalogue &transport_catalogue,
                            const std::string_view stopname_to_output, Logger *output) const;

        void parse_perform_stat_queries(TransportCatalogue &transport_catalogue,
                                        const int n, Logger *output = nullptr);
    };
}