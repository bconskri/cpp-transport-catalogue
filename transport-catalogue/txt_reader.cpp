#include "txt_reader.h"
#include "request_handler.h"
#include "domain.h"

#include <queue>
#include <string_view>
#include <string>
#include <sstream>
#include <fstream>

namespace txt_reader {

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

    void TxtData::PerfomUploadQueries(TransportCatalogue &transport_catalogue, request_handler::Inputer *input) {
        int queries_count;
        auto& input_ = dynamic_cast<TxtData *>(input)->GetStream();
        input_ >> queries_count;
        input_.ignore(1);

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

    void TxtData::OutputStopInfo(TransportCatalogue &transport_catalogue,
                                 const std::string_view stopname_to_output, request_handler::Logger *output) const {
        using namespace std::literals;

        std::optional<std::set<std::string_view>> buses_for_stop = transport_catalogue.GetBusesForStopInfo(
                stopname_to_output);
        std::ostringstream stream;
        if (buses_for_stop == std::nullopt) {
            stream << "Stop "s << stopname_to_output << ": not found"s << std::endl;
        } else if (buses_for_stop->empty()) {
            stream << "Stop "s << stopname_to_output << ": no buses"s << std::endl;
        } else {
            stream << "Stop "s << stopname_to_output << ": buses ";
            bool first = true;
            for (auto bus: *buses_for_stop) {
                if (first) {
                    stream << bus;
                    first = false;
                } else {
                    stream << " "s << bus;
                }
            }
            stream << std::endl;
        }
        output->log(stream.str());
    }

    void TxtData::OutputBusInfo(TransportCatalogue &transport_catalogue,
                                const std::string_view busname_to_output, request_handler::Logger *output) const {
        using namespace std::literals;
        auto route = transport_catalogue.GetRouteInfo(busname_to_output);
        //
        if (route != nullptr) {
            std::ostringstream stream;
            stream << "Bus "s << route->name << ": " << route->stops_on_route << " stops on route, "s <<
                   route->unique_stops << " unique stops, "s << std::fixed << std::setprecision(6) <<
                   route->route_length_meters << " route length, " << std::fixed << std::setprecision(6)
                   << route->curvature << " curvature"s << std::endl;
            output->log(stream.str());
        } else {
            std::ostringstream stream;
            stream << "Bus "s << busname_to_output << ": not found" << std::endl;
            output->log(stream.str());
        }
    }

    void TxtData::parse_perform_stat_queries(TransportCatalogue &transport_catalogue,
                                             const int q_count, request_handler::Inputer *input,
                                             request_handler::Logger *output) {
        using namespace std::literals;
        auto& input_ = dynamic_cast<TxtData *>(input)->GetStream();

        std::string line;
        std::string_view line_to_parse;
        for (int i = 0; i < q_count; ++i) {
            std::getline(input_, line, '\n');
            line_to_parse = Trim(line);
            if (!line_to_parse.empty()) {
                //split into two parts devide by _
                const size_t pos = line_to_parse.find_first_of(' ');
                if (pos != line.npos) { //: found - that is upload query
                    auto query_type = line_to_parse.substr(0, pos);
                    auto query_data = Trim(line_to_parse.substr(pos + 1, line_to_parse.npos));
                    //
                    if (query_type == "Bus"sv) {
                        OutputBusInfo(transport_catalogue, query_data, output);

                    } else if (query_type == "Stop"sv) {
                        OutputStopInfo(transport_catalogue, query_data, output);

                    } else {
                        throw ("Query_type/name parse error");
                    }
                }
            } else {
                throw ("That is not upload query");
            }
        }
    }


    void TxtData::PerfomStatQueries(TransportCatalogue &transport_catalogue, request_handler::Inputer *input,
                                    request_handler::Logger *output) {
        int queries_count;
        auto& input_ = dynamic_cast<TxtData *>(input)->GetStream();
        input_ >> queries_count;
        input_.ignore(1);

        if (output == nullptr) {
            output = new request_handler::ConsoleLogger();
        }
        this->parse_perform_stat_queries(transport_catalogue, queries_count, input, output);
    }

    std::istream &TxtData::GetStream() {
        return std::cin;
    }

    void TxtData::PerfomQueries(TransportCatalogue &transport_catalogue, request_handler::Inputer *input,
                       request_handler::Logger *output) {
        TxtData::PerfomUploadQueries(transport_catalogue, input);
        TxtData::PerfomStatQueries(transport_catalogue, input, output);
    }

} //namespace txt_reader