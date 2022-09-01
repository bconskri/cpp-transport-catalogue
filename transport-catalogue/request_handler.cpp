#include "request_handler.h"
#include "txt_reader.h"

#include "json_reader.h"

#include <string>
#include <cassert>

namespace request_handler {
    QueryHandler *QueryHandler::GetHandler(const io_type datatype) {
        QueryHandler *p = nullptr;
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
        Logger *p = nullptr;
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
        Inputer *p =nullptr;
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

    void QueryHandler::LinkCatalogue(const TransportCatalogue &catalogue_) {
        transport_catalogue_ = std::make_shared<TransportCatalogue>(catalogue_);
    }

    void QueryHandler::MapRender([[maybe_unused]] request_handler::Logger *output) {
        map_render_->SetStops(transport_catalogue_->GetStops());
        map_render_->SetRoutes(transport_catalogue_->GetRoutes());
        //
        map_render_->Render(std::cout); //fixme - replace on outputer stream
    }

    void ConsoleLogger::log(const std::string_view &msg) {
        std::cout << msg;
    }

    FileLogger::FileLogger(const std::string &filename) {
        ofs.open(filename);
    }

    FileLogger::~FileLogger() {
        if (ofs) {
            ofs.close();
        }
    }

    void FileLogger::log(const std::string_view &msg) {
        ofs << msg;
    }

    FileInputer::FileInputer(const std::string &filename) {
        ofs_.open(filename);
    }

    FileInputer::~FileInputer() {
        if (ofs_) {
            ofs_.close();
        }
    }

    std::ifstream &FileInputer::GetStream() {
        return ofs_;
    }

    std::istream &ConsoleInputer::GetStream() {
        return std::cin;
    }
} //namespace request_handler