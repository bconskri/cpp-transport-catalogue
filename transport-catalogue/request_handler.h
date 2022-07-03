#pragma once

#include "transport_catalogue.h"
#include "map_renderer.h"

#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <memory>

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * В качестве источника для идей предлагаем взглянуть на нашу версию обработчика запросов.
 * Вы можете реализовать обработку запросов способом, который удобнее вам.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

// Класс RequestHandler играет роль Фасада, упрощающего взаимодействие JSON reader-а
// с другими подсистемами приложения.
// См. паттерн проектирования Фасад: https://ru.wikipedia.org/wiki/Фасад_(шаблон_проектирования)
/*
class RequestHandler {
public:
    // MapRenderer понадобится в следующей части итогового проекта
    RequestHandler(const TransportCatalogue& db, const renderer::MapRenderer& renderer);

    // Возвращает информацию о маршруте (запрос Bus)
    std::optional<BusStat> GetBusStat(const std::string_view& bus_name) const;

    // Возвращает маршруты, проходящие через
    const std::unordered_set<BusPtr>* GetBusesByStop(const std::string_view& stop_name) const;

    // Этот метод будет нужен в следующей части итогового проекта
    svg::Document RenderMap() const;

private:
    // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
    const TransportCatalogue& db_;
    const renderer::MapRenderer& renderer_;
};
*/

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

        virtual std::istream &GetStream() = 0;
    };

    class ConsoleInputer final : public Inputer {
    public:
        std::istream &GetStream() override {
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

        std::ifstream &GetStream() override {
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
        QueryHandler()
                : transport_catalogue_(std::make_shared<TransportCatalogue>()),
                  map_render_(std::make_shared<map_renderer::MapRender>()) {};

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
        void LinkCatalogue(const TransportCatalogue &catalogue_);

        //map renderer
        void MapRender(request_handler::Logger *output = nullptr);

    protected:
        std::shared_ptr<TransportCatalogue> transport_catalogue_;
        std::shared_ptr<map_renderer::MapRender> map_render_;
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