#include "stat_reader.h"
#include "input_reader.h"

#include <queue>
#include <iomanip>
#include <set>
#include <sstream>

namespace stat_reader {
    void StreamData::OutputStopInfo(TransportCatalogue &transport_catalogue,
                                    const std::string_view stopname_to_output, Logger *output) const {
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

    void StreamData::OutputBusInfo(TransportCatalogue &transport_catalogue,
                                   const std::string_view busname_to_output, Logger *output) const {
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

    void StreamData::parse_perform_stat_queries(TransportCatalogue &transport_catalogue,
                                                const int lcount, Logger *output) {
        using namespace std::literals;

        std::string line;
        std::string_view line_to_parse;
        for (int i = 0; i < lcount; ++i) {
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


    void StreamData::PerfomStatQueries(TransportCatalogue &transport_catalogue, Logger *output) {
        int queries_count;
        this->input_ >> queries_count;
        this->input_.ignore(1);

        if (output == nullptr) {
            output = new ConsoleLogger();
        }
        this->parse_perform_stat_queries(transport_catalogue, queries_count, output);
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
}