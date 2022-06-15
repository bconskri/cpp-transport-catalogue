#include "input_reader.h"

#include <queue>
#include <string_view>
#include <string>
#include <sstream>

namespace input_reader {

    std::pair<std::string, unsigned> ParseDistanceToStop(std::string_view line) {
        auto pos = line.find_first_of('m');
        unsigned meters = std::stoi(std::string(line.substr(0, pos)));

        pos = line.find_first_not_of(' ', pos + 1);
        line.remove_prefix(pos + 3);

        return std::make_pair(std::string(Ltrim(line.substr(0,
                                                            line.npos))), meters);
    }

    std::istream &operator>>(std::istream &input, BusStop &bus_stop) {
        std::string line;
        std::getline(input, line);

        std::string_view line_to_parse = line;
        auto pos = line_to_parse.find_first_of(':');

        if (pos == line_to_parse.npos) {
            input.ignore(1);
            return input;
        }
        //extract name
        bus_stop.name = line_to_parse.substr(0, pos);
        line_to_parse.remove_prefix(pos + 1);
        //we have rest of string after :
        pos = line_to_parse.find_first_of(',');
        bus_stop.coords.lat = std::stod(std::string(Trim(line_to_parse.substr(0, pos))));
        line_to_parse.remove_prefix(pos + 1);
        //проверим если можно рассплитовать правую часть по запятым, значит есть расстояния
        pos = line_to_parse.find_first_of(',');
        if (pos == line.npos) {
            //запятую не нашли - все забираем в lng
            bus_stop.coords.lng = std::stod(std::string(Trim(line_to_parse)));
        } else {
            //запятую нашли все что левее - долгота, правее - остановки
            bus_stop.coords.lng = std::stod(std::string(Trim(line_to_parse.substr(0, pos))));
            line_to_parse.remove_prefix(pos + 1);
            //долготу убрали остались только остановки - обработаем их
            while (pos != line_to_parse.npos) {
                pos = line_to_parse.find_first_of(',');
                bus_stop.distance_to_other_stops.insert(
                        ParseDistanceToStop(line_to_parse.substr(0, pos)));

                line_to_parse.remove_prefix(pos + 1);
            }
        }
        //input.ignore(1);
        return input;
    }

    std::istream &operator>>(std::istream &input, BusRoute &bus_route) {
        std::string line;
        std::getline(input, line);

        std::string_view line_to_parse = line;
        auto pos = line_to_parse.find_first_of(':');

        if (pos == line_to_parse.npos) {
            input.ignore(1);
            return input;
        }
        //extract name
        bus_route.name = line_to_parse.substr(0, pos);
        line_to_parse.remove_prefix(pos + 1);
        //we have rest of string after :
        char stops_delimetr;
        if (line_to_parse.find_first_of('-') != line_to_parse.npos &&
            line_to_parse.find_first_of('>') == line_to_parse.npos) {
            //its forward route
            bus_route.is_circular = false;
            stops_delimetr = '-';

        } else if (line_to_parse.find_first_of('>') != line_to_parse.npos &&
                   line_to_parse.find_first_of('-') == line_to_parse.npos) {
            //its circle route
            bus_route.is_circular = true;
            stops_delimetr = '>';

        } else {
            throw ("Error route description& It has - and > simultaneously");
        }
        //parsing of stops in route
        while (pos != line_to_parse.npos) {
            pos = line_to_parse.find_first_of(stops_delimetr);
            auto stop_on_route = std::string(Trim(line_to_parse.substr(0, pos)));
            bus_route.stops.push_back(stop_on_route);
            bus_route.unique_stops.insert(stop_on_route);

            line_to_parse.remove_prefix(pos + 1);
        }

        //input.ignore(1);
        return input;
    }

    void StreamData::PerfomUploadQueries(TransportCatalogue &transport_catalogue) {
        int queries_count;
        this->input_ >> queries_count;
        this->input_.ignore(1);

        std::string line, query;
        for (int i = 0; i < queries_count; ++i) {
            std::getline(input_, line, '\n');

            std::stringstream buffer(line);

            buffer >> query;
            buffer.ignore(1);

            if (query == "Stop") {
                BusStop stop;
                buffer >> stop;
                transport_catalogue.AddStop(std::move(stop));
            } else if (query == "Bus") {
                BusRoute route;
                buffer >> route;
                transport_catalogue.AddRoute(std::move(route));
            }
        }

        //recalc route stats after all data added
        transport_catalogue.CalcRoutesStat();
    }

    QueryHandler *QueryHandler::GetHandler(const io_type datasearch, std::istream &input) {
        QueryHandler *p;
        switch (datasearch) {
            case Console:
                p = new StreamData(input);
                break;

            default:
                assert(false);
        }
        return p;
    }
}