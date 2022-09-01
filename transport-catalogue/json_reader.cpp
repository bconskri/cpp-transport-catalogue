#include "json_reader.h"
#include "json_builder.h"
#include "request_handler.h"
#include "transport_catalogue.h"
#include "domain.h"
#include "serialization.h"

#include <sstream>

using namespace std::literals;

namespace json_reader {
    void JsonData::ParseStop(json::Node &request) {
        BusStop stop;

        auto stop_json = request.AsDict();
        stop.name = stop_json.at("name").AsString();
        stop.coordinates = {stop_json.at("latitude").AsDouble(),
                            stop_json.at("longitude").AsDouble()};

        for (auto &pair: stop_json.at("road_distances").AsDict()) {
            stop.distance_to_other_stops[pair.first] = pair.second.AsInt();
        }
        if (serializer_->FileDefined()) {
            serializer_->Serialize(stop);
        }
        transport_catalogue_->AddStop(std::move(stop));
    }

    void JsonData::ParseBus(json::Node &request) {
        BusRoute route;
        auto route_json = request.AsDict();
        route.name = route_json.at("name").AsString();

        route.is_roundtrip = route_json.at("is_roundtrip").AsBool();

        for (auto &stop: route_json.at("stops").AsArray()) {
            route.stops.push_back(stop.AsString());
            route.unique_stops.insert(stop.AsString());
        }
        if (serializer_->FileDefined()) {
            serializer_->Serialize(route);
        }
        transport_catalogue_->AddRoute(std::move(route));
    }

    void JsonData::ParsePerformUploadQueries(std::vector<json::Node> &upload_requests) {
        for (auto &request: upload_requests) {
            auto request_ = request.AsDict();
            if (request_.at("type").AsString() == "Stop") {
                ParseStop(request);
            } else if (request_.at("type").AsString() == "Bus") {
                ParseBus(request);
            }
        }
        //recalc route stats after all data added
        transport_catalogue_->CalcRoutesStat();
        //std::cout << "ParsePerformUploadQueries Complete" << std::endl;
    }

    svg::Color NodeAsColor(json::Node &node) {
        if (node.IsArray()) {
            if (node.AsArray().size() == 4) {
                svg::Rgba rgba;
                rgba.red = node.AsArray()[0].AsInt();
                rgba.green = node.AsArray()[1].AsInt();
                rgba.blue = node.AsArray()[2].AsInt();
                rgba.opacity = node.AsArray()[3].AsDouble();
                return rgba;
            } else {
                svg::Rgb rgb;
                rgb.red = node.AsArray()[0].AsInt();
                rgb.green = node.AsArray()[1].AsInt();
                rgb.blue = node.AsArray()[2].AsInt();
                return rgb;
            }
        } else {
            return node.AsString();
        }
    }

    void JsonData::ParseRenderSettings(json::Dict &render_settings) {
        using namespace std::literals;
        RenderSettings settings;

        settings.width = render_settings.at("width"s).AsDouble();
        settings.height = render_settings.at("height"s).AsDouble();

        settings.padding = render_settings.at("padding"s).AsDouble();

        settings.line_width = render_settings.at("line_width"s).AsDouble();
        settings.stop_radius = render_settings.at("stop_radius"s).AsDouble();

        settings.bus_label_font_size = render_settings.at("bus_label_font_size"s).AsInt();
        settings.bus_label_offset[0] = render_settings.at("bus_label_offset"s).AsArray()[0].AsDouble();
        settings.bus_label_offset[1] = render_settings.at("bus_label_offset"s).AsArray()[1].AsDouble();

        settings.stop_label_font_size = render_settings.at("stop_label_font_size"s).AsInt();
        settings.stop_label_offset[0] = render_settings.at("stop_label_offset"s).AsArray()[0].AsDouble();
        settings.stop_label_offset[1] = render_settings.at("stop_label_offset"s).AsArray()[1].AsDouble();

        settings.underlayer_color = NodeAsColor(render_settings.at("underlayer_color"s));
        settings.underlayer_width = render_settings.at("underlayer_width"s).AsDouble();

        settings.underlayer_width = render_settings.at("underlayer_width"s).AsDouble();
        for (auto &node: render_settings.at("color_palette"s).AsArray()) {
            settings.color_palette.emplace_back(NodeAsColor(node));
        }

        if (serializer_->FileDefined()) {
            serializer_->Serialize(settings);
        }

        map_render_->SetSettings(settings);
    }

    void JsonData::ParseRoutingSettings(json::Dict &routing_settings) {
        using namespace std::literals;
        RoutingSettings settings;
        const double KPH_TO_MPM = 1000.0 / 60.0;

        settings.bus_wait_time = routing_settings.at("bus_wait_time"s).AsDouble();
        settings.bus_velocity = routing_settings.at("bus_velocity"s).AsDouble() * KPH_TO_MPM;

        if (serializer_->FileDefined()) {
            serializer_->Serialize(settings);
        }

        route_manager_->SetSettings(settings);
    }

    void JsonData::PerformSerializerSettings(json::Dict &serializer_settings) {
        SerializerSettings settings;
        settings.file = serializer_settings.at("file"s).AsString();

        serializer_->SetSettings(settings);
    }

    void JsonData::PerfomUploadQueries(request_handler::Inputer *input) {
        auto root = json::Load(input->GetStream()).GetRoot();
        //sprint 14 new query - serialization_settings
        if (root.AsDict().count("serialization_settings")) {
            PerformSerializerSettings(root.AsDict().at("serialization_settings").AsDict());
        }
        //
        if (root.AsDict().count("render_settings")) {
            ParseRenderSettings(root.AsDict().at("render_settings").AsDict());
        }
        //sprint12 add routing
        if (root.AsDict().count("routing_settings")) {
            ParseRoutingSettings(root.AsDict().at("routing_settings").AsDict());
        }
        //
        ParsePerformUploadQueries(root.AsDict().at("base_requests").AsArray());
        //sprint 14 we must serialize output to file
        if (serializer_->FileDefined()) {
            //sprint14 task 3/3 we need to build graph and route_manager and serialize them
            if (transport_catalogue_->GetStopsCount() > 0) {
                serializer_->Serialize(transport_catalogue_, route_manager_);
            }
            //
            serializer_->FlushToFile();
        }
    }

    void JsonData::PerfomStatQueries(request_handler::Inputer *input,
                                     [[maybe_unused]] request_handler::Logger *output) {
        auto root = json::Load(input->GetStream()).GetRoot();
        //
        //sprint 14 new query - serialization_settings
        if (root.AsDict().count("serialization_settings")) {
            PerformSerializerSettings(root.AsDict().at("serialization_settings").AsDict());
            //need to deserialize db and load catalogue and settings
            if (serializer_->FileDefined()) {
                serializer_->Deserialize(transport_catalogue_, route_manager_, map_render_);
            }
        }
        //
        ParsePerformStatQueries(root.AsDict().at("stat_requests").AsArray());
        //выводим Node json
        json::PrintNode(output_json_builder_.Build(), std::cout); //fixme outputer
    }

    void JsonData::PerfomQueries(request_handler::Inputer *input,
                                 [[maybe_unused]] request_handler::Logger *output) {
        auto root = json::Load(input->GetStream()).GetRoot();
        //
        ParsePerformUploadQueries(root.AsDict().at("base_requests").AsArray());
        if (root.AsDict().count("render_settings")) {
            ParseRenderSettings(root.AsDict().at("render_settings").AsDict());
        }
        //sprint12 add routing
        if (root.AsDict().count("routing_settings")) {
            ParseRoutingSettings(root.AsDict().at("routing_settings").AsDict());
        }
        //
        ParsePerformStatQueries(root.AsDict().at("stat_requests").AsArray());
        //выводим Node json
        json::PrintNode(output_json_builder_.Build(), std::cout); //fixme outputer
    }

    void JsonData::ParsePerformStatQueries(std::vector<json::Node> &stat_requests) {
        output_json_builder_.StartArray();

        for (auto &request: stat_requests) {
            using namespace std::literals;
            auto request_ = request.AsDict();

            output_json_builder_.StartDict().Key("request_id"s).Value(request_.at("id").AsInt());

            if (request_.at("type").AsString() == "Bus") {
                PerformBusQuery(request);
            } else if (request_.at("type").AsString() == "Stop") {
                PerformStopQuery(request);
            } else if (request_.at("type").AsString() == "Map") {
                PerformMapQuery();
                //Спринт 12. В список stat_requests добавляются элементы с "type": "Route" —
                // это запросы на построение маршрута между двумя остановками
            } else if (request_.at("type").AsString() == "Route") {
                PerformRouteQuery(request);
            }
            output_json_builder_.EndDict();
        }
        output_json_builder_.EndArray();
    }

    void JsonData::PerformRouteQuery(json::Node &request) {
        auto route_build = route_manager_->BuildRoute(transport_catalogue_,
                                                      request.AsDict().at("from"s).AsString(),
                                                      request.AsDict().at("to"s).AsString());
        //
        if (route_build) {
            output_json_builder_.Key("total_time"s).Value(route_build.value().first);
            output_json_builder_.Key("items"s).StartArray();
            for (const auto &item: route_build.value().second) {
                output_json_builder_.StartDict();
                if (item.type == RouteSegmentType::Wait) {
                    output_json_builder_.Key("stop_name"s).Value(item.stop_name);
                    output_json_builder_.Key("time"s).Value(item.time);
                    output_json_builder_.Key("type"s).Value("Wait"s);
                } else {
                    output_json_builder_.Key("bus"s).Value(item.bus);
                    output_json_builder_.Key("span_count"s).Value(item.span_count);
                    output_json_builder_.Key("time"s).Value(item.time);
                    output_json_builder_.Key("type"s).Value("Bus"s);
                }
                output_json_builder_.EndDict();
            }
            output_json_builder_.EndArray();
        } else {
            output_json_builder_.Key("error_message"s).Value("not found"s);
        }
    }

    void JsonData::PerformMapQuery() {
        std::ostringstream map_rendered;
        map_render_->SetStops(transport_catalogue_->GetStops());
        map_render_->SetRoutes(transport_catalogue_->GetRoutes());
        //
        map_render_->Render(map_rendered);
        //
        output_json_builder_.Key("map").Value(json::Node(map_rendered.str()));
    }

    void JsonData::PerformBusQuery(json::Node &request) {
        using namespace std::literals;
        auto route = transport_catalogue_->GetRouteInfo(request.AsDict().at("name").AsString());

        if (route != nullptr) {
            output_json_builder_.Key("curvature").Value(json::Node(route->curvature))
                    .Key("route_length").Value(json::Node(route->route_length_meters))
                    .Key("stop_count").Value(json::Node(int(route->stops_on_route)))
                    .Key("unique_stop_count").Value(json::Node(int(route->unique_stops)));
        } else {
            output_json_builder_.Key("error_message").Value(json::Node("not found"s));
        }
    }

    void JsonData::PerformStopQuery(json::Node &request) {
        using namespace std::literals;
        std::optional<std::set<std::string_view>> buses_for_stop = transport_catalogue_->GetBusesForStopInfo(
                request.AsDict().at("name").AsString());

        if (buses_for_stop == std::nullopt) {
            output_json_builder_.Key("error_message").Value(json::Node("not found"s));
        } else if (buses_for_stop->empty()) {
            output_json_builder_.Key("buses"s).Value(json::Node(json::Array{}));
        } else {
            output_json_builder_.Key("buses"s).StartArray();
            //
            for (auto bus: *buses_for_stop) {
                output_json_builder_.Value(std::string(bus));
            }
            output_json_builder_.EndArray();
        }
    }
} // namespace txt_reader