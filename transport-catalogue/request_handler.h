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
        static Inputer *GetInputer(const io_stream datasearch, std::string file_name = "");

        virtual std::istream& GetStream() = 0;
    };

    class ConsoleInputer final : public Inputer {
    public:
        std::istream& GetStream() override {
            return std::cin;
        }
    };

    class FileInputer final : public Inputer {
    public:
        FileInputer(const std::string &filename) {
            ofs_.open(filename);
        }

        ~FileInputer() {
            if (ofs_) {
                ofs_.close();
            }
        }

        std::ifstream& GetStream() override {
            return ofs_;
        }

    private:
        std::ifstream ofs_;
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
        virtual void PerfomUploadQueries(request_handler::Inputer *input) = 0;

        virtual void PerfomStatQueries(request_handler::Inputer *input,
                                       request_handler::Logger *output = nullptr) = 0;

        virtual void PerfomQueries(request_handler::Inputer *input,
                                   request_handler::Logger *output = nullptr) = 0;

        //create loader from specified data search
        static QueryHandler *GetHandler(const io_type datatype);

        //привязывает транспортный каталог для работы с ним
        void LinkCatalogue(const TransportCatalogue& catalogue_);
    protected:
        std::shared_ptr<TransportCatalogue> transport_catalogue_;
    };

    template<typename T>
    Inputer &operator>>(Inputer &classObj, T into) {
        classObj.GetStream() >> into;
        return classObj;
    }
} //namespace request_handler

inline std::string_view Ltrim(std::string_view str) {
    const auto pos(str.find_first_not_of(" \t\n\r\f\v"));
    str.remove_prefix(std::min(pos, str.length()));
    return str;
}

inline std::string_view Rtrim(std::string_view str) {
    const auto pos(str.find_last_not_of(" \t\n\r\f\v"));
    str.remove_suffix(std::min(str.length() - pos - 1, str.length()));
    return str;
}

inline std::string_view Trim(std::string_view str) {
    str = Ltrim(str);
    str = Rtrim(str);
    return str;
}