#include "stat_reader.h"
#include "input_reader.h"

#include <queue>
#include <iomanip>

namespace stat_reader {
    void StreamData::parse_perform_stat_queries(TransportCatalogue &transport_catalogue,
                                                const int lcount, Logger* output) {
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
                    const std::string query_data = std::string(Trim(line.substr(pos + 1, line.npos)));
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
                std::ostringstream stream;
                stream << "Bus "s << route->name << ": " << route->stops_on_route << " stops on route, "s <<
                       route->unique_stops << " unique stops, "s << std::setprecision(6) <<
                       route->route_length << " route length"
                       << std::endl;
                output->log(stream.str());
            } else {
                std::ostringstream stream;
                stream << "Bus "s << name << ": not found" << std::endl;
                output->log(stream.str());
            }
            buses_to_output.pop();
        }
    }

    void StreamData::PerfomStatQueries(TransportCatalogue &transport_catalogue, Logger* output) {
        int n;
        std::string line;
        std::getline(input_, line, '\n');
        n = std::stoi(std::string(Trim(line)));

        if (output == nullptr) {
            output = new ConsoleLogger();
        }
        this->parse_perform_stat_queries(transport_catalogue, n, output);
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