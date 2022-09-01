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

    struct RouteData {
        double weight;
        std::optional<graph::EdgeId> prev_edge;
    };

    void Serializer::Deserialize(std::shared_ptr<TransportCatalogue> transport_catalogue,
                                 std::shared_ptr<transport_router::RouteManager> route_manager,
                                 std::shared_ptr<map_renderer::MapRender> map_render) {
        if (FileDefined()) {
            std::fstream ifs(settings_.file, std::ios::binary | std::ios::in);
            if (ifs.is_open()) {
                transport_package_.mutable_catalogue()->Clear();
                if (transport_package_.ParseFromIstream(&ifs)) {
                    //Deserialize transport catalogue
                    {
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
                    }
                    //Deserialize settings
                    //Deserialize routing settings
                    {
                        auto route_set_deserialized = transport_package_.routing_settings();
                        RoutingSettings rout_settings;
                        rout_settings.bus_wait_time = route_set_deserialized.bus_wait_time();
                        rout_settings.bus_velocity = route_set_deserialized.bus_velocity();
                        route_manager->SetSettings(rout_settings);
                        //
                    }
                    //Deserialize render settings
                    {
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
                    }
                    //Deserialize graph and router
                    //auto route_build = route_manager->BuildRoute(transport_catalogue, "", "");
                    //
                    if (transport_catalogue->GetStopsCount() > 0) {
                        auto route_manager_pack = transport_package_.route_manager();
                        //deSerialize stop_to_vertex
                        {
                            std::unordered_map<std::string, graph::VertexId> stop_to_vertex;
                            for (int i = 0; i < route_manager_pack.stop_to_vertex_size(); ++i) {
                                const auto stop_to_vertex_pack = route_manager_pack.stop_to_vertex(i);
                                stop_to_vertex.emplace(stop_to_vertex_pack.name(), stop_to_vertex_pack.vertex_id());
                            }
                            route_manager->stop_to_vertex_.swap(stop_to_vertex);
                        }
                        //deSerialize edge_to_route_segment_
                        {
                            std::unordered_map<graph::EdgeId, RouteEdgeInfo> edge_to_route_segment;
                            /* struct RouteEdgeInfo {
                                std::string stop_name;
                                std::string bus;
                                int span_count;
                                double time;
                                RouteSegmentType type;
                            */
                            for (int i = 0; i < route_manager_pack.edge_to_route_segment_size(); ++i) {
                                const auto edge_to_route_segment_pack = route_manager_pack.edge_to_route_segment(i);
                                RouteEdgeInfo route_edge_info;
                                route_edge_info.stop_name = edge_to_route_segment_pack.route_edge_info().stop_name();
                                route_edge_info.bus = edge_to_route_segment_pack.route_edge_info().bus();
                                route_edge_info.span_count = edge_to_route_segment_pack.route_edge_info().span_count();
                                route_edge_info.time = edge_to_route_segment_pack.route_edge_info().time();
                                route_edge_info.type = edge_to_route_segment_pack.route_edge_info().type() ?
                                                       RouteSegmentType::Bus : RouteSegmentType::Wait;
                                edge_to_route_segment.emplace(edge_to_route_segment_pack.edge_id(), route_edge_info);
                            }
                            route_manager->edge_to_route_segment_.swap(edge_to_route_segment);
                        }
                        //deSerialize graph_
                        {
                            std::vector<graph::Edge<double>> edges;
                            /*struct Edge {
                                VertexId from;
                                VertexId to;
                                Weight weight;
                            */
                            for (int i = 0; i < route_manager_pack.graph().edges_size(); ++i) {
                                auto edge_pack = route_manager_pack.graph().edges(i);
                                auto &add_edge = edges.emplace_back();
                                add_edge.from = edge_pack.from();
                                add_edge.to = edge_pack.to();
                                add_edge.weight = edge_pack.weight();
                            }
                            std::vector<std::vector<graph::EdgeId>> incidence_lists;
                            //using EdgeId = size_t;
                            for (int i = 0; i < route_manager_pack.graph().incidence_lists_size(); ++i) {
                                auto incidence_lists_pack = route_manager_pack.graph().incidence_lists(i);
                                auto &incidence_lists_element = incidence_lists.emplace_back();
                                for (int j = 0; j < incidence_lists_pack.edge_id_size(); ++j) {
                                    incidence_lists_element.push_back(incidence_lists_pack.edge_id(j));
                                }
                            }
                            route_manager->graph_ = graph::DirectedWeightedGraph<double>(std::move(edges),
                                                                                         std::move(incidence_lists));
                        }
                        //deSerialize router_
                        {
                            graph::Router<double>::RoutesInternalData routes_internal_data;
                            for (int i = 0;
                                 i < route_manager_pack.router().routes_internal_data().routes_internal_data_size();
                                 ++i) { //std::vector<
                                auto routes_internal_data_pack =
                                        route_manager_pack.router().routes_internal_data().routes_internal_data(i);
                                auto &routes_internal_data_element = routes_internal_data.emplace_back();
                                for (int j = 0; j < routes_internal_data_pack.route_internal_data_vector_size(); ++j) {
                                    //std::vector<std::vector<
                                    auto &element = routes_internal_data_element.emplace_back();
                                    auto route_internal_data_vector_element = routes_internal_data_pack.route_internal_data_vector(
                                            j);
                                    if (route_internal_data_vector_element.nullopt() == 0) {
                                        graph::Router<double>::RouteInternalData data_to_set;
                                        data_to_set.weight = route_internal_data_vector_element.weight();
                                        if (route_internal_data_vector_element.prev_edge_set()) {
                                            data_to_set.prev_edge = route_internal_data_vector_element.prev_edge();
                                        }
                                        element = data_to_set;
                                    }
                                }
                            }
                            //свопить приват поля приводит к плавающей ошибке в тренажере
                            //сделал через конструктор
                            route_manager->router_ = std::make_unique<graph::Router<double>>(
                                    route_manager->graph_.value(),
                                    std::move(routes_internal_data));
                        }
                    }
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
        //use NULL query for build only
        auto route_build = route_manager->BuildRoute(transport_catalogue, "", "");
        //
        //Serialize stop_to_vertex
        auto &route_manager_pack = *transport_package_.mutable_route_manager();
        for (const auto&[key, value] : route_manager->stop_to_vertex_) {
            auto stop_to_vertex = route_manager_pack.add_stop_to_vertex();
            stop_to_vertex->set_name(key);
            stop_to_vertex->set_vertex_id(value);
        };
        //Serialize edge_to_route_segment_
        for (const auto&[key, value] : route_manager->edge_to_route_segment_) {
            auto edge_to_route_segment = route_manager_pack.add_edge_to_route_segment();
            edge_to_route_segment->set_edge_id(key);
            edge_to_route_segment->mutable_route_edge_info()->set_stop_name(value.stop_name);
            edge_to_route_segment->mutable_route_edge_info()->set_bus(value.bus);
            edge_to_route_segment->mutable_route_edge_info()->set_span_count(value.span_count);
            edge_to_route_segment->mutable_route_edge_info()->set_time(value.time);
            if (value.type == RouteSegmentType::Wait) {
                edge_to_route_segment->mutable_route_edge_info()->set_type(0);
            } else {
                edge_to_route_segment->mutable_route_edge_info()->set_type(1);
            }
        };
        //Serialize graph_
        auto &edges = route_manager->graph_->edges_; //std::vector<Edge<Weight>> edges_;
        auto &mutable_graph = *route_manager_pack.mutable_graph();
        //add edges
        for (const auto &e: edges) {
            auto &add_edges = *mutable_graph.add_edges();
            add_edges.set_from(e.from);
            add_edges.set_to(e.to);
            add_edges.set_weight(e.weight);
        }
        //add incidence_lists
        auto &incidence_lists = route_manager->graph_->incidence_lists_; //std::vector<IncidenceList> incidence_lists_;
        for (const auto &e: incidence_lists) {
            auto &add_incidence_lists = *mutable_graph.add_incidence_lists();
            for (const auto &id : e) {
                add_incidence_lists.add_edge_id(id);
            }
        }
        //Serialize router_
        /* std::vector<std::vector<std::optional<RouteInternalData>>>;
         * struct RouteInternalData {
         * Weight weight;
         * std::optional<EdgeId> prev_edge;
         * };
         */
        auto &routes_internal_data = route_manager->router_->routes_internal_data_;
        auto &mutable_router = *route_manager_pack.mutable_router();
        auto &mutable_routes_internal_data = *mutable_router.mutable_routes_internal_data();
        for (const auto &rd : routes_internal_data) {
            //std::vector<
            auto &add_routes_internal_data = *mutable_routes_internal_data.add_routes_internal_data();
            for (const auto &rdv : rd) {
                //std::vector<std::vector<
                auto &add_route_internal_data_vector = *add_routes_internal_data.add_route_internal_data_vector();
                if (rdv.has_value()) {
                    add_route_internal_data_vector.set_nullopt(0); //optional<RouteInternalData>
                    add_route_internal_data_vector.set_weight(rdv->weight);
                    if (rdv->prev_edge.has_value()) {
                        add_route_internal_data_vector.set_prev_edge_set(1); //optional prev_edge
                        add_route_internal_data_vector.set_prev_edge(rdv->prev_edge.value());
                    } else {
                        add_route_internal_data_vector.set_prev_edge_set(0); //optional prev_edge
                    }
                } else {
                    add_route_internal_data_vector.set_nullopt(1); //optional<RouteInternalData>
                }
            }
        }
    }
} //namespace serialization