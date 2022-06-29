#include "request_handler.h"
#include "txt_reader.h"
#include "json_reader.h"

#include <string>
#include <cassert>

namespace request_handler {
    QueryHandler *QueryHandler::GetHandler(const io_type datatype) {
        QueryHandler *p;
        switch (datatype) {
            case Txt:
                p = new txt_reader::TxtData();
                break;

            case Json:
                p = new json_reader::JsonData();
                break;

            default:
                assert(false);
        }
        return p;
    }

    Logger *Logger::GetLogger(const io_stream datasearch, std::string file_name) {
        Logger *p;
        switch (datasearch) {
            case Console:
                p = new ConsoleLogger();
                break;

            case File:
                p = new FileLogger(file_name);
                break;

            default:
                assert(false);
        }
        return p;
    }

    Inputer *Inputer::GetInputer(const io_stream datasearch, std::string file_name) {
        Inputer *p;
        switch (datasearch) {
            case Console:
                p = new ConsoleInputer();
                break;

            case File:
                p = new FileInputer(file_name);
                break;

            default:
                assert(false);
        }
        return p;
    }
} //namespace console_reader