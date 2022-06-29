#pragma once

#include "transport_catalogue.h"

#include <iostream>
#include <fstream>
#include <string>
#include <string_view>

namespace request_handler {
    enum io_stream {
        Console = 0, File
    };

    enum io_type {
        Txt = 0, Json, Xml
    };

    class Inputer {
    public:
        virtual void input(const std::string_view &) = 0;

        static Inputer *GetInputer(const io_stream datasearch, std::string file_name = "");
    };

    class ConsoleInputer final : public Inputer {
    public:
        void input(const std::string_view &msg) override {
            std::cout << msg;
        };
    };

    class FileInputer final : public Inputer {
    public:
        FileInputer(const std::string &filename) {
            ofs.open(filename);
        }

        ~FileInputer() {
            if (ofs) {
                ofs.close();
            }
        }

        void input(const std::string_view &msg) override {
            ofs << msg;
        }

    private:
        std::ofstream ofs;
    };

    class Logger {
    public:
        virtual void log(const std::string_view &) = 0;

        static Logger *GetLogger(const io_stream datasearch, std::string file_name = "");
    };

    class ConsoleLogger final : public Logger {
    public:
        void log(const std::string_view &msg) override {
            std::cout << msg;
        };
    };

    class FileLogger final : public Logger {
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
        virtual void PerfomUploadQueries(TransportCatalogue &transport_catalogue, request_handler::Inputer *input) = 0;

        virtual void PerfomStatQueries(TransportCatalogue &transport_catalogue, request_handler::Inputer *input,
                                       request_handler::Logger *output = nullptr) = 0;

        virtual void PerfomQueries(TransportCatalogue &transport_catalogue, request_handler::Inputer *input,
                                   request_handler::Logger *output = nullptr) = 0;

        //create loader from specified data search
        static QueryHandler *GetHandler(const io_type datatype);
    };
} //namespace request_handler