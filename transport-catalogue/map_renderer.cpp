#include "map_renderer.h"
#include "request_handler.h"

#include "svg.h"
#include "domain.h"
#include "geo.h"

#include <memory>

namespace map_renderer {
    void MapRender::SetSettings(const RenderSettings &settings) {
        settings_ = settings;
    }

    void MapRender::SetRoutes(const std::unordered_map<std::string_view, BusRoute *> &routes) {
        routes_ = routes;
    }

    void MapRender::SetStops(const std::unordered_map<std::string_view, BusStop *> &stops) {
        stops_ = stops;
    }

    void MapRender::RenderBusRoutesLinesLayer(const SphereProjector &projector,
                                              std::set<const BusRoute *, LexSort<BusRoute>> &routes_) {
        int index = 0;
        for (const auto &route: routes_) {
            svg::Polyline line;
            for (const auto &stop: route->stops) {
                line.AddPoint(projector(stops_.at(stop)->coordinates));
            }
            line.SetFillColor("none");
            line.SetStrokeColor(settings_.color_palette[index % settings_.color_palette.size()]);
            line.SetStrokeWidth(settings_.line_width);
            line.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
            line.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
            index++;
            doc_.Add(line);
        }
    }

    void MapRender::RenderRoutesNamesLayer(const SphereProjector &projector,
                                           std::set<const BusRoute *, LexSort<BusRoute>> &routes_) {
        using namespace std::literals;
        int index = 0;
        svg::Text underlying, text;
        underlying.SetOffset({settings_.bus_label_offset[0], settings_.bus_label_offset[1]})
                .SetFontSize(settings_.bus_label_font_size)
                .SetFontFamily("Verdana"s)
                .SetFontWeight("bold"s)

                .SetFillColor(settings_.underlayer_color)
                .SetStrokeColor(settings_.underlayer_color)
                .SetStrokeWidth(settings_.underlayer_width)
                .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        //
        text.SetOffset({settings_.bus_label_offset[0], settings_.bus_label_offset[1]})
                .SetFontSize(settings_.bus_label_font_size)
                .SetFontFamily("Verdana"s)
                .SetFontWeight("bold"s);
        //
        for (const auto &route: routes_) {
            std::vector<std::string_view> stops_for_labeling{route->stops.front()};
            if (!route->is_roundtrip && route->stops.front() != route->end_stop) {
                stops_for_labeling.emplace_back(route->end_stop);
            }
            for (const auto &stop: stops_for_labeling) {
                underlying.SetPosition(projector(stops_.at(stop)->coordinates))
                        .SetData(route->name);

                doc_.Add(underlying);
                //
                text.SetPosition(projector(stops_.at(stop)->coordinates))
                        .SetData(route->name)
                        .SetFillColor(settings_.color_palette[index % settings_.color_palette.size()]);

                doc_.Add(text);
            }
            index++;
        }
    }

    void MapRender::RenderStopsLayer(const SphereProjector &projector,
                                     std::set<const BusStop *, LexSort<BusStop>> &stops_to_render) {
        using namespace std::literals;
        svg::Circle circle;
        circle.SetRadius(settings_.stop_radius)
                .SetFillColor("white"s);

        for (const auto &stop: stops_to_render) {
            //if (!stop->buses.empty()) {
            circle.SetCenter(projector(stop->coordinates));

            doc_.Add(circle);
            //}
        }
    }

    void MapRender::RenderStopsNamesLayer(const SphereProjector &projector,
                                          std::set<const BusStop *, LexSort<BusStop>> &stops_to_render) {
        using namespace std::literals;
        svg::Text underlying, text;
        underlying.SetOffset({settings_.stop_label_offset[0], settings_.stop_label_offset[1]})
                .SetFontSize(settings_.stop_label_font_size)
                .SetFontFamily("Verdana"s)

                .SetFillColor(settings_.underlayer_color)
                .SetStrokeColor(settings_.underlayer_color)
                .SetStrokeWidth(settings_.underlayer_width)
                .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

        text.SetOffset({settings_.stop_label_offset[0], settings_.stop_label_offset[1]})
                .SetFontSize(settings_.stop_label_font_size)
                .SetFontFamily("Verdana"s)
                .SetFillColor("black");

        for (const auto &stop: stops_to_render) {
            //if (!stop.second->buses.empty()) {
            underlying.SetPosition(projector(stop->coordinates))
                    .SetData(stop->name);
            doc_.Add(underlying);
            //
            text.SetPosition(projector(stop->coordinates))
                    .SetData(stop->name);
            doc_.Add(text);
            //}
        }
    }

    void MapRender::Render(std::ostream &output) {
        std::vector<geo::Coordinates> coordinates;
        //coordinates.reserve(10'000);
        std::set<const BusStop *, LexSort<BusStop>> stops_to_render;
        for (const auto &[name, stop]: stops_) {
            if (!stop->buses.empty()) {
                coordinates.emplace_back(stop->coordinates);
                stops_to_render.emplace(stop);
            }
        }
        SphereProjector projector(coordinates.begin(), coordinates.end(),
                                  settings_.width, settings_.height, settings_.padding);
        //
        std::set<const BusRoute *, LexSort<BusRoute>> routes_to_render;
        for (const auto &route: routes_) {
            if (!route.second->stops.empty()) {
                routes_to_render.emplace(route.second);
            }
        }
        //
        RenderBusRoutesLinesLayer(projector, routes_to_render);
        RenderRoutesNamesLayer(projector, routes_to_render);
        RenderStopsLayer(projector, stops_to_render);
        RenderStopsNamesLayer(projector, stops_to_render);
        //
        doc_.Render(output);
    }
} //map_render