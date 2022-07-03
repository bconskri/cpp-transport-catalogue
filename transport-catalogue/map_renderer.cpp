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

    void MapRender::RenderBusRoutes(SphereProjector &projector,
                                    std::set<const BusRoute *, BusRouteSort> &routes_) {
        int color_index = 0;
        for (const auto &route: routes_) {
            svg::Polyline line;
            for (const auto &stop: route->stops) {
                line.AddPoint(projector(stops_.at(stop)->coordinates));
            }
            line.SetFillColor("none");
            line.SetStrokeColor(settings_.color_palette[color_index % settings_.color_palette.size()]);
            line.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
            line.SetStrokeWidth(settings_.line_width);
            line.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
            color_index++;
            doc_.Add(line);
        }
    }

    void MapRender::Render(std::ostream &output) {
        std::vector<geo::Coordinates> coordinates;
        for (const auto &[name, stop]: stops_) {
            if (!stop->buses.empty()) {
                coordinates.emplace_back(stop->coordinates);
            }
        }
        SphereProjector projector(coordinates.begin(), coordinates.end(),
                                  settings_.width, settings_.height, settings_.padding);
        //
        std::set<const BusRoute *, BusRouteSort> routes_to_render;
        for (const auto &route: routes_) {
            if (!route.second->stops.empty()) {
                routes_to_render.insert(route.second);
            }
        }
        //
        RenderBusRoutes(projector, routes_to_render);
        //
        doc_.Render(output);
    }
} //map_render