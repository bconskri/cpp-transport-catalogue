// напишите решение с нуля
// код сохраните в свой git-репозиторий
#include "input_reader.h"
#include "utilities.h"
#include <string>
#include <string_view>

namespace input_reader {
    void StreamData::perform_write_queries(const TransportCatalogue &transport_catalogue,
                                           const int n,
                                           std::istream& input) {
        std::string line, query;
        for (int i = 0; i < n; ++i)
        {
            std::getline(input, line, '\n');
            line = utilities::trim(line);
            if (!line.empty()) {
                const size_t pos = line.find_first_of(':');
                if (pos != line.npos) {
                    const std::string_view query_type_name = line.substr(0, pos);
                    const std::string_view query_data = line.substr(pos+1, );
                    //
                    const size_t pos = query_type_name.find_first_of(' ');
                    if (pos != line.npos) {
                        const std::string_view query_type = line.substr(0, pos);
                        const std::string_view name = trim(line.substr(pos+1, ));
                        //
                        transport_catalogue.AddStop()
                    } else {
                        assert(false);
                    }
                } else {
                    assert(false);
                }
            }

            std::stringstream buffer(line);

            buffer >> query;
            buffer.ignore(1);

            if (query == "Stop")
            {
                Stop stop;
                buffer >> stop;
                AddStop(stop);
            }

            if (query == "Bus")
            {
                std::string route_description;
                std::getline(buffer, route_description, '\n');

                AddRoute(route_description);
            }
        }
    }

//    void StreamData::perform_read_queries(const int n, std::istream& input, std::ostream *output) {
//
//    }

    void StreamData::LoadPerfomQueries(const TransportCatalogue &transport_catalogue) {
        int n;
        input_ >> n;
        input_.ignore(1);

        perform_write_queries(transport_catalogue, n, input_);

        //routes_database_.BuildAllRoutes(stops_database_);

        input_ >> n;
        input_.ignore(1);

        perform_read_queries(transport_catalogue, n, input_, output_);
    }
}