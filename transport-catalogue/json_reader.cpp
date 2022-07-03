#include "json_reader.h"

#include "request_handler.h"
#include "transport_catalogue.h"
#include "json.h"
#include "domain.h"

namespace json_reader {
    void JsonData::parse_stop(json::Node &request) {
        BusStop stop;

        auto stop_json = request.AsMap();
        stop.name = stop_json.at("name").AsString();
        stop.coordinates = {stop_json.at("latitude").AsDouble(),
                            stop_json.at("longitude").AsDouble()};

        for (auto &pair: stop_json.at("road_distances").AsMap()) {
            stop.distance_to_other_stops[pair.first] = pair.second.AsInt();
        }
        transport_catalogue_->AddStop(std::move(stop));
    }

    void JsonData::parse_bus(json::Node &request) {
        BusRoute route;
        auto route_json = request.AsMap();
        route.name = route_json.at("name").AsString();

        if (route_json.at("is_roundtrip").AsBool()) {
            route.is_roundtrip = true;
        } else {
            route.is_roundtrip = false;
        }

        for (auto &stop: route_json.at("stops").AsArray()) {
            route.stops.push_back(stop.AsString());
            route.unique_stops.insert(stop.AsString());
        }

        transport_catalogue_->AddRoute(std::move(route));
    }

    void JsonData::parse_perform_upload_queries(std::vector<json::Node> &upload_requests) {
        for (auto &request: upload_requests) {
            auto request_ = request.AsMap();
            if (request_.at("type").AsString() == "Stop") {
                parse_stop(request);
            } else if (request_.at("type").AsString() == "Bus") {
                parse_bus(request);
            }
        }
        //recalc route stats after all data added
        transport_catalogue_->CalcRoutesStat();
    }

    void JsonData::PerfomUploadQueries(request_handler::Inputer *input) {
        auto root = json::Load(input->GetStream()).GetRoot();
        parse_perform_upload_queries(root.AsMap().at("base_requests").AsArray());
    }

    void JsonData::PerfomStatQueries(request_handler::Inputer *input,
                                     [[maybe_unused]] request_handler::Logger *output) {
        auto root = json::Load(input->GetStream()).GetRoot();
        //создаем пустую структуру ответа json Node
        // в корне ответа в формате json лежит вектор Array
        root_ = json::Node(json::Array{});
        parse_perform_stat_queries(root.AsMap().at("stat_requests").AsArray());
        //выводим Node json
        json::PrintNode(root_, std::cout); //fixme outputer
    }

    void JsonData::PerfomQueries(request_handler::Inputer *input,
                                 [[maybe_unused]] request_handler::Logger *output) {
        auto root = json::Load(input->GetStream()).GetRoot();
        root_ = json::Node(json::Array{});
        parse_perform_upload_queries(root.AsMap().at("base_requests").AsArray());
        parse_perform_stat_queries(root.AsMap().at("stat_requests").AsArray());
        //выводим Node json
        json::PrintNode(root_, std::cout); //fixme outputer
    }

    void JsonData::parse_perform_stat_queries(std::vector<json::Node> &stat_requests) {
        for (auto &request: stat_requests) {
            using namespace std::literals;
            json::Dict response;
            auto request_ = request.AsMap();
            response["request_id"s] = request_.at("id").AsInt();

            if (request_.at("type").AsString() == "Bus") {
                perform_bus_query(request, response);
            } else if (request_.at("type").AsString() == "Stop") {
                perform_stop_query(request, response);
            }
            root_.AsArray().push_back(json::Node(std::move(response)));
        }
    }

    void JsonData::perform_bus_query(json::Node &request, json::Dict &response) {
        using namespace std::literals;
        auto route = transport_catalogue_->GetRouteInfo(request.AsMap().at("name").AsString());

        if (route != nullptr) {
            response["curvature"] = json::Node(route->curvature);
            response["route_length"] = json::Node(route->route_length_meters);
            response["stop_count"] = json::Node(int(route->stops_on_route));
            response["unique_stop_count"] = json::Node(int(route->unique_stops));

        } else {
            response["error_message"] = json::Node("not found"s);
        }
    }

    void JsonData::perform_stop_query(json::Node &request, json::Dict &response) {
        using namespace std::literals;
        std::optional<std::set<std::string_view>> buses_for_stop = transport_catalogue_->GetBusesForStopInfo(
                request.AsMap().at("name").AsString());

        if (buses_for_stop == std::nullopt) {
            response["error_message"s] = json::Node("not found"s);
        } else if (buses_for_stop->empty()) {
            response["buses"s] = json::Node(json::Array{});
        } else {
            json::Array buses;
            //
            for (auto bus: *buses_for_stop) {
                buses.push_back(std::string(bus));
            }
            response["buses"] = json::Node(std::move(buses));
        }
    }
} // namespace txt_reader