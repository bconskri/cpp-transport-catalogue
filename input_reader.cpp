// напишите решение с нуля
// код сохраните в свой git-репозиторий
#include "input_reader.h"

#include <queue>
#include <string_view>
#include <string>

namespace input_reader {
    void
    StreamData::PerformStopDistanceUpload(TransportCatalogue &transport_catalogue,
                                          std::queue<std::pair<std::string, std::string>>& from_to_distances) {
        using namespace std::literals;

        while (!from_to_distances.empty()) {
            auto elem = from_to_distances.front();
            //
            auto from_stop = transport_catalogue.GetStopByName(elem.first);
            if (from_stop == nullptr) {
                throw("Error adding distance. Stop from doesn't added");
            }

            auto distances = SplitByChar(std::string(elem.second), ',');

            for (auto dist: distances) {
                const size_t pos = dist.find_first_of('m');
                if (pos != dist.npos) {
                    const auto meters = Rtrim(dist.substr(0, pos));
                    auto stop_name = Rtrim(dist.substr(pos + 1, dist.npos));
                    auto pos = stop_name.find("to"s);
                    stop_name = Ltrim(stop_name.substr(pos + 3, dist.npos));
                    const Stop *to_stop = transport_catalogue.GetStopByName(stop_name);
                    if (to_stop == nullptr) {
                        throw("Error adding distance. Stop to doesn't added");
                    }
                    auto t = std::stoi(meters);
                    //std::cout << "From " << from_stop->name << " to " << to_stop->name << " m " << meters << std::endl;
                    transport_catalogue.AddDistance(from_stop, to_stop, static_cast<size_t>(t));
                }
            }
            //
            from_to_distances.pop();
        }
        //std::cerr << transport_catalogue.distances_to_stops_;
    }

        void StreamData::parse_perform_upload_queries(TransportCatalogue &transport_catalogue,
                                                      const int lcount) {
            using namespace std::literals;

            std::queue<std::pair<std::string, std::string>> routes_to_add;
            std::queue<std::pair<std::string, std::string>> from_to_distances;
            std::string line;

            for (int i = 0; i < lcount; ++i) {
                std::getline(input_, line, '\n');
                line = Trim(line);
                if (!line.empty()) {
                    //splitinto twi parts devide by :
                    const size_t pos_colon = line.find_first_of(':');
                    if (pos_colon != line.npos) { //: found - that is upload query
                        const auto query_type_name = line.substr(0, pos_colon);
                        const auto query_data = line.substr(pos_colon + 1, line.npos);
                        //
                        //split into two parts devide by " " for name extruction
                        const size_t pos_space = query_type_name.find_first_of(' ');
                        if (pos_space != line.npos) {
                            const auto query_type = query_type_name.substr(0, pos_space);
                            const auto name =
                                    std::string(Trim(query_type_name.substr(pos_space + 1, query_type_name.npos)));
                            //
                            if (query_type == "Stop"s) {
                                //we have query to add stop
                                // we neeed to parse coordinates
                                //split into two parts devide by ","
                                const size_t pos_comma = query_data.find_first_of(',');
                                if (pos_comma != line.npos) {
                                    Stop stop_to_upload;
                                    stop_to_upload.name = std::move(name);
                                    stop_to_upload.coords.lat = std::stod(
                                            Rtrim(query_data.substr(0, pos_comma)));
                                    //проверим если можно рассплитовать правую часть по запятым, значит есть расстояния
                                    auto lng_string = Ltrim(query_data.substr(pos_comma + 1, line.npos));
                                    const size_t pos_lng = lng_string.find_first_of(',');
                                    if (pos_lng != line.npos) {
                                        //запятую нашли все что левее - долгота, правее - остановки
                                        auto stops_string = Ltrim(lng_string.substr(pos_lng + 1, line.npos));
                                        lng_string = Rtrim(lng_string.substr(0, pos_lng));
                                        //запомним необходимое расстояние в стек
                                        from_to_distances.push(
                                                {stop_to_upload.name, stops_string});

                                    }
                                    stop_to_upload.coords.lng = std::stod(lng_string);
                                    //add stop to transport catalogue
                                    transport_catalogue.AddStop(std::move(stop_to_upload));

                                } else {
                                    throw ("Error in coordinate format");
                                }
                            } else if ((query_type == "Bus"s)) {
                                //we have query to add route
                                //parse route and add to queue
                                routes_to_add.push({std::string(name), query_data});
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
            //now we can add all distances
            PerformStopDistanceUpload(transport_catalogue, from_to_distances);
            //we processed all strings
            //we added all stops
            //we added all distances
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
            n = std::stoi(Trim(line));

            this->parse_perform_upload_queries(transport_catalogue, n);
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