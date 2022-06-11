// напишите решение с нуля
// код сохраните в свой git-репозиторий
#include "input_reader.h"

void StreamData::parse_perform_upload_queries(const TransportCatalogue &transport_catalogue,
                                              const int lcount,
                                              std::istream& input) {
    using namespace std::literals;

    std::string line, query;
    for (int i = 0; i < lcount; ++i)
    {
        std::getline(input, line, '\n');
        line = Trim(line);
        if (!line.empty()) {
            const size_t pos = line.find_first_of(':');
            if (pos != line.npos) { //: found - that is upload query
                const std::string_view query_type_name = line.substr(0, pos);
                const std::string_view query_data = line.substr(pos+1, line.npos);
                //
                const size_t pos = query_type_name.find_first_of(' ');
                if (pos != line.npos) {
                    const std::string_view query_type = line.substr(0, pos);
                    const std::string_view name = Trim(line.substr(pos + 1, line.npos));
                    //
                    if (query_type=="Stop"sv) {

                    } else if ((query_type=="Bus"sv)) {

                    } else {
                        throw("Unknown upload query type");
                    }
                } else {
                    throw("Query_type/name parse error");
                }
            } else {
                throw("That is not upload query");
            }
        }
    }
}

void StreamData::PerfomUploadQueries(const TransportCatalogue &transport_catalogue) {
    int n;
    this->input_ >> n;
    std::cerr << n;
    this->input_.ignore(1);

    this->parse_perform_upload_queries(transport_catalogue, n, input_);
}

QueryPerformer* QueryPerformer::GetHandler(const input_type datasearch, std::istream& input = std::cin) {
    QueryPerformer *p;
    switch (datasearch) {
        case Dstream:
            p = new StreamData(input);
            break;

        default:
            assert(false);
    }
    return p;
};