#include "serialization.h"

namespace serialization {
    void serialization::Serializer::SetSettings(const SerializerSettings &settings) {
        settings_ = settings;
    }

    bool Serializer::FileDefined() const {
        return !settings_.file.empty();
    }

    void Serializer::Serialize(const BusRoute &route) {
        auto &route_ = *transport_package_.mutable_catalogue()->add_routes();
        route_.set_name(route.name);
        route_.set_is_roundtrip(route.is_roundtrip);
        for (const auto &e: route.stops) {
            route_.add_stops(e);
        }
    }

    void Serializer::Serialize(const BusStop &stop) {
        auto &stop_ = *transport_package_.mutable_catalogue()->add_stops();
        stop_.set_name(stop.name);
        auto &coordinates = *stop_.mutable_coordinates();
        coordinates.set_lat(stop.coordinates.lat);
        coordinates.set_lng(stop.coordinates.lng);
        for (const auto &e: stop.distance_to_other_stops) {
            auto &distance = *stop_.add_distances();
            distance.set_stop_name(e.first);
            distance.set_distance(e.second);
        }
    }

    void Serializer::FlushToFile() {
        if (FileDefined()) {
            std::fstream ofs(settings_.file, std::ios::binary | std::ios::out);
            if (ofs.is_open()) {
                transport_package_.SerializeToOstream(&ofs);
            }
        }
    }

    void Serializer::Deserialize(std::shared_ptr<TransportCatalogue> transport_catalogue,
                                 std::shared_ptr<transport_router::RouteManager> route_manager,
                                 std::shared_ptr<map_renderer::MapRender> map_render) {
        if (FileDefined()) {
            std::fstream ifs(settings_.file, std::ios::binary | std::ios::in);
            if (ifs.is_open()) {
                transport_package_.mutable_catalogue()->Clear();
                if (transport_package_.ParseFromIstream(&ifs)) {
                    //Deserialize transport catalogue
                    auto &catalogue = *transport_package_.mutable_catalogue();
                    //Deserialize stops
                    for (int i = 0; i < catalogue.stops_size(); i++) {
                        const auto &stop = catalogue.stops(i);
                        BusStop stop_;
                        stop_.name = stop.name();
                        stop_.coordinates = geo::Coordinates{stop.coordinates().lat(),
                                                             stop.coordinates().lng()};
                        for (int j = 0; j < stop.distances_size(); ++j) {
                            auto distance = stop.distances(j);
                            stop_.distance_to_other_stops.emplace(distance.stop_name(), distance.distance());
                        }
                        transport_catalogue->AddStop(std::move(stop_));
                    }
                    //Deserialize routes
                    for (int i = 0; i < catalogue.routes_size(); i++) {
                        const auto &route = catalogue.routes(i);
                        BusRoute route_;
                        route_.name = route.name();
                        route_.is_roundtrip = route.is_roundtrip();
                        for (int j = 0; j < route.stops_size(); ++j) {
                            route_.stops.emplace_back(route.stops(j));
                            //это тоже заполняется прежде чем вызвать AddRoute
                            route_.unique_stops.insert(route.stops(j));
                        }
                        transport_catalogue->AddRoute(std::move(route_));
                    }
                    //auto &routes_ = transport_package_.mutable_catalogue()->route();
                    //recalc route stats after all data added
                    transport_catalogue->CalcRoutesStat();
                    //
                    //Deserialize settings
                    //
                    //Deserialize routing settings
                    auto route_set_deserialized = transport_package_.routing_settings();
                    RoutingSettings rout_settings;
                    rout_settings.bus_wait_time = route_set_deserialized.bus_wait_time();
                    rout_settings.bus_velocity = route_set_deserialized.bus_velocity();
                    route_manager->SetSettings(rout_settings);
                    //
                    //Deserialize render settings
                    auto render_set_deserialized = transport_package_.render_settings();
                    RenderSettings rend_settings;
                    rend_settings.width = render_set_deserialized.width();
                    rend_settings.height = render_set_deserialized.height();
                    rend_settings.padding = render_set_deserialized.padding();
                    rend_settings.line_width = render_set_deserialized.line_width();
                    rend_settings.stop_radius = render_set_deserialized.stop_radius();
                    rend_settings.bus_label_font_size = render_set_deserialized.bus_label_font_size();
                    rend_settings.bus_label_offset[0] = render_set_deserialized.bus_label_offset(0);
                    rend_settings.bus_label_offset[1] = render_set_deserialized.bus_label_offset(1);
                    rend_settings.stop_label_font_size = render_set_deserialized.stop_label_font_size();
                    rend_settings.stop_label_offset[0] = render_set_deserialized.stop_label_offset(0);
                    rend_settings.stop_label_offset[1] = render_set_deserialized.stop_label_offset(1);
                    //
                    //render_set_deserialized.underlayer_color().has_str_color()
                    if (render_set_deserialized.underlayer_color().str_rgb_rgba_case() ==
                        transport_package::Color::kStrColor) {
                        rend_settings.underlayer_color = render_set_deserialized.underlayer_color().str_color();

                    } else if (render_set_deserialized.underlayer_color().has_rgb()) {
                        svg::Rgb rgb;
                        rgb.red = render_set_deserialized.underlayer_color().rgb().red();
                        rgb.green = render_set_deserialized.underlayer_color().rgb().green();
                        rgb.blue = render_set_deserialized.underlayer_color().rgb().blue();
                        rend_settings.underlayer_color = rgb;

                    } else if (render_set_deserialized.underlayer_color().has_rgba()) {
                        svg::Rgba rgba;
                        rgba.red = render_set_deserialized.underlayer_color().rgba().rgb().red();
                        rgba.green = render_set_deserialized.underlayer_color().rgba().rgb().green();
                        rgba.blue = render_set_deserialized.underlayer_color().rgba().rgb().blue();
                        rgba.opacity = render_set_deserialized.underlayer_color().rgba().opacity();
                        rend_settings.underlayer_color = rgba;
                    }
                    //
                    rend_settings.underlayer_width = render_set_deserialized.underlayer_width();
                    //
                    for (int i = 0; i < render_set_deserialized.color_palette_size(); i++) {
                        const auto &color = render_set_deserialized.color_palette(i);
                        //
                        //color.has_str_color()
                        if (color.str_rgb_rgba_case() == transport_package::Color::kStrColor) {
                            rend_settings.color_palette.push_back(color.str_color());

                        } else if (color.has_rgb()) {
                            svg::Rgb rgb;
                            rgb.red = color.rgb().red();
                            rgb.green = color.rgb().green();
                            rgb.blue = color.rgb().blue();
                            rend_settings.color_palette.push_back(rgb);

                        } else if (color.has_rgba()) {
                            svg::Rgba rgba;
                            rgba.red = color.rgba().rgb().red();
                            rgba.green = color.rgba().rgb().green();
                            rgba.blue = color.rgba().rgb().blue();
                            rgba.opacity = color.rgba().opacity();
                            rend_settings.color_palette.push_back(rgba);
                        }
                    }
                    map_render->SetSettings(rend_settings);
                    //
                    //Deserialize graph and router

                }
            }
        }
    }

    void Serializer::Serialize(const RenderSettings &settings) {
        auto &render_ = *transport_package_.mutable_render_settings();
        render_.set_width(settings.width);
        render_.set_height(settings.height);
        render_.set_padding(settings.padding);
        render_.set_line_width(settings.line_width);
        render_.set_stop_radius(settings.stop_radius);
        render_.set_bus_label_font_size(settings.bus_label_font_size);
        //
        render_.add_bus_label_offset(settings.bus_label_offset[0]);
        render_.add_bus_label_offset(settings.bus_label_offset[1]);
        //
        render_.set_stop_label_font_size(settings.stop_label_font_size);
        //
        render_.add_stop_label_offset(settings.stop_label_offset[0]);
        render_.add_stop_label_offset(settings.stop_label_offset[1]);
        //
        if (settings.underlayer_color.has_value()) {
            if (std::holds_alternative<std::string>(settings.underlayer_color.value())) {
                *render_.mutable_underlayer_color()->mutable_str_color() =
                        std::get<std::string>(settings.underlayer_color.value());
            } else if (std::holds_alternative<svg::Rgb>(settings.underlayer_color.value())) {
                svg::Rgb c = std::get<svg::Rgb>(settings.underlayer_color.value());
                auto &rgb = *render_.mutable_underlayer_color()->mutable_rgb();
                rgb.set_red(c.red);
                rgb.set_green(c.green);
                rgb.set_blue(c.blue);
            } else if (std::holds_alternative<svg::Rgba>(settings.underlayer_color.value())) {
                svg::Rgba c = std::get<svg::Rgba>(settings.underlayer_color.value());
                auto &rgba = *render_.mutable_underlayer_color()->mutable_rgba();
                rgba.mutable_rgb()->set_red(c.red);
                rgba.mutable_rgb()->set_green(c.green);
                rgba.mutable_rgb()->set_blue(c.blue);
                rgba.set_opacity(c.opacity);
            }
        }
        //
        render_.set_underlayer_width(settings.underlayer_width);
        //
        for (const auto &c: settings.color_palette) {
            if (c.has_value()) {
                auto &color_ = *render_.add_color_palette();
                //
                if (std::holds_alternative<std::string>(c.value())) {
                    *color_.mutable_str_color() = std::get<std::string>(c.value());

                } else if (std::holds_alternative<svg::Rgb>(c.value())) {
                    svg::Rgb rgb_c = std::get<svg::Rgb>(c.value());
                    color_.mutable_rgb()->set_red(rgb_c.red);
                    color_.mutable_rgb()->set_green(rgb_c.green);
                    color_.mutable_rgb()->set_blue(rgb_c.blue);

                } else if (std::holds_alternative<svg::Rgba>(c.value())) {
                    svg::Rgba rgba_c = std::get<svg::Rgba>(c.value());
                    color_.mutable_rgba()->mutable_rgb()->set_red(rgba_c.red);
                    color_.mutable_rgba()->mutable_rgb()->set_green(rgba_c.green);
                    color_.mutable_rgba()->mutable_rgb()->set_blue(rgba_c.blue);
                    color_.mutable_rgba()->set_opacity(rgba_c.opacity);
                }
            }
        }
    }

    void Serializer::Serialize(const RoutingSettings &settings) {
        auto &routing_ = *transport_package_.mutable_routing_settings();
        routing_.set_bus_wait_time(settings.bus_wait_time);
        routing_.set_bus_velocity(settings.bus_velocity);
    }

    void Serializer::Serialize(std::shared_ptr<TransportCatalogue> transport_catalogue,
                               std::shared_ptr<transport_router::RouteManager> route_manager) {
        //we need to build graph and router
        //use NULL query
        auto route_build = route_manager->BuildRoute(transport_catalogue, "", "");
        //
//        //VertexId надо где-то хранить, так как библиотека Graph работает только с ребрами
//        std::unordered_map<std::string, graph::VertexId> stop_to_vertex_;
//        //маршрутизатор возвращает нам номера ребер графа - по которым проложен маршрут
//        //нужно уметь восстановить по номеру ребра все данные для вывода в отчет
//        std::unordered_map<graph::EdgeId, RouteEdgeInfo> edge_to_route_segment_;
//        //Graph надо строить 1 раз. Сделаем LazyValue механизм
//        std::optional<graph::DirectedWeightedGraph<double>> graph_;
//        //здесь будем хранить роутер
//        std::unique_ptr<graph::Router<double>> router_;
    }
} //namespace serialization