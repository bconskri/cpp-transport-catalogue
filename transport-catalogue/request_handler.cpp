#include "request_handler.h"
#include "console_reader.h"

#include <iostream>
#include <string>
#include <cassert>

namespace request_handler {
    QueryHandler *QueryHandler::GetHandler(const io_type datasearch, std::istream &input) {
        QueryHandler *p;
        switch (datasearch) {
            case Console:
                p = new console_reader::StreamData(input);
                break;

            default:
                assert(false);
        }
        return p;
    }

    Logger *Logger::GetLogger(const io_type datasearch, std::string file_name) {
        Logger *p;
        switch (datasearch) {
            case Console:
                p = new ConsoleLogger();
                break;

            case Txt:
                p = new FileLogger(file_name);
                break;

            default:
                assert(false);
        }
        return p;
    }
} //namespace console_reader