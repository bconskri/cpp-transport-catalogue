#include "stat_reader.h"

// напишите решение с нуля
// код сохраните в свой git-репозиторий
#include "input_reader.h"

#include <queue>
#include <iomanip>

namespace stat_reader {
    void StreamData::parse_perform_stat_queries(TransportCatalogue &transport_catalogue,
                                                const int lcount) {
        using namespace std::literals;

        std::queue<std::string> buses_to_output;
        std::string line;

        for (int i = 0; i < lcount; ++i) {
            std::getline(input_, line, '\n');
            line = Trim(line);
            if (!line.empty()) {
                //split into two parts devide by _
                const size_t pos = line.find_first_of(' ');
                if (pos != line.npos) { //: found - that is upload query
                    const std::string query_type = line.substr(0, pos);
                    const std::string query_data = line.substr(pos + 1, line.npos);
                    //
                    if (query_type == "Bus"s) {
                        buses_to_output.emplace(std::move(query_data));

                    } else {
                        throw ("Query_type/name parse error");
                    }
                } else {
                    throw ("That is not upload query");
                }
            }
        }
        //we processed all strings
        //we added all stops
        //now we need to process all routes from queue
        while (!buses_to_output.empty()) {
            auto name = buses_to_output.front();
            auto route = transport_catalogue.GetRouteInfo(name);
            //
            if (route != nullptr) {
                output_ << "Bus "s << route->name << ": " << route->stops_on_route << " stops on route, "s <<
                        route->unique_stops << " unique stops, "s << std::setprecision(6) <<
                        route->route_length << " route length"
                        << std::endl;
            } else {
                output_ << "Bus "s << name << ": not found" << std::endl;
            }
            buses_to_output.pop();
        }
    }

    void StreamData::PerfomStatQueries(TransportCatalogue &transport_catalogue) {
        int n;
        this->input_ >> n;
        this->input_.ignore(1);

        this->parse_perform_stat_queries(transport_catalogue, n);
    }

    QueryHandler *QueryHandler::GetHandler(const io_type datasearch, std::istream &input = std::cin) {
        QueryHandler *p;
        switch (datasearch) {
            case Dstream:
                p = new StreamData(input);
                break;

            default:
                assert(false);
        }
        return p;
    }
}