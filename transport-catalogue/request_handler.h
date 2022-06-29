#pragma once

#include "transport_catalogue.h"

#include <iostream>
#include <fstream>
#include <string>
#include <string_view>

namespace request_handler {
    enum io_type {
        Console = 0, Txt, Json, Xml
    };

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
        virtual void PerfomUploadQueries(TransportCatalogue &transport_catalogue) = 0;
        virtual void PerfomStatQueries(TransportCatalogue &transport_catalogue, Logger *output = nullptr) = 0;

        //create loader from specified data search
        static QueryHandler *GetHandler(const io_type datasearch, std::istream &input = std::cin);
    };
} //namespace request_handler