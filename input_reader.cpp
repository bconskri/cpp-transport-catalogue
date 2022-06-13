// напишите решение с нуля
// код сохраните в свой git-репозиторий
#include "input_reader.h"

#include <queue>

namespace input_reader {
    void StreamData::parse_perform_upload_queries(TransportCatalogue &transport_catalogue,
                                                  const int lcount) {
        using namespace std::literals;

        std::queue<std::pair<std::string, std::string>> routes_to_add;
        std::string line;

        for (int i = 0; i < lcount; ++i) {
            std::getline(input_, line, '\n');
            line = Trim(line);
            if (!line.empty()) {
                //splitinto twi parts devide by :
                const size_t pos = line.find_first_of(':');
                if (pos != line.npos) { //: found - that is upload query
                    const std::string query_type_name = line.substr(0, pos);
                    const std::string query_data = line.substr(pos + 1, line.npos);
                    //
                    //split into two parts devide by " " for name extruction
                    const size_t pos = query_type_name.find_first_of(' ');
                    if (pos != line.npos) {
                        const std::string query_type = query_type_name.substr(0, pos);
                        const std::string name = std::string(
                                Trim(query_type_name.substr(pos + 1, query_type_name.npos)));
                        //
                        if (query_type == "Stop"s) {
                            //we have query to add stop
                            // we neeed to parse coordinates
                            //splitinto twi parts devide by ","
                            const size_t pos = query_data.find_first_of(',');
                            if (pos != line.npos) {
                                Stop stop_to_upload;
                                stop_to_upload.name = std::move(name);
                                stop_to_upload.coords.lat = std::stod(std::string(Rtrim(query_data.substr(0, pos))));
                                stop_to_upload.coords.lng = std::stod(
                                        std::string(Ltrim(query_data.substr(pos + 1, line.npos))));
                                //add stop to transport catalogue
                                transport_catalogue.AddStop(std::move(stop_to_upload));

                            } else {
                                throw ("Error in coordinate format");
                            }
                        } else if ((query_type == "Bus"s)) {
                            //we have query to add route
                            //parse route and add to queue
                            routes_to_add.push({name, query_data});
                        } else {
                            throw ("Unknown upload query type");
                        }
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
        while (!routes_to_add.empty()) {
            auto route = routes_to_add.front();
            //need to parse route
            //make resulting structure
            BusRoute route_to_add;
            route_to_add.name = route.first;
            //std::clog << "Route forming: "s << route.first << std::endl;
            //
            std::vector<std::string> stops_in_route;

            if (route.second.find_first_of('-') != route.second.npos &&
                route.second.find_first_of('>') == route.second.npos) {
                //its forward route
                route_to_add.is_circular = false;
                stops_in_route = SplitByChar(route.second, '-');

            } else if (route.second.find_first_of('>') != route.second.npos &&
                       route.second.find_first_of('-') == route.second.npos) {
                //its circle route
                route_to_add.is_circular = true;
                stops_in_route = SplitByChar(route.second, '>');

            } else {
                throw ("Error route description& It has - and > simultaneously");
            }
            std::for_each(stops_in_route.begin(), stops_in_route.end(),
                          [transport_catalogue, &route_to_add](const std::string stop_name) {
                              const auto stop = transport_catalogue.GetStopByName(stop_name);
                              if (stop != nullptr) {
                                  route_to_add.stops.emplace_back(stop);
                                  //std::clog << "Stop added: "s << stop->name << std::endl;
                              } else {
                                  throw ("Unknown stop in route");
                              }
                          });
            //adding route
            transport_catalogue.AddRoute(std::move(route_to_add));
            routes_to_add.pop();
        }
    }

    void StreamData::PerfomUploadQueries(TransportCatalogue &transport_catalogue) {
        int n;
//        this->input_ >> n;
//        this->input_.ignore(1);
        std::string line;
        std::getline(input_, line, '\n');
        n = std::stoi(std::string(Trim(line)));

        this->parse_perform_upload_queries(transport_catalogue, n);
    }

    QueryHandler *QueryHandler::GetHandler(const io_type datasearch, std::istream &input = std::cin) {
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