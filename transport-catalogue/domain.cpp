#include "domain.h"

RouteInfo::RouteInfo(std::string_view name, size_t stops_count, size_t unique_stops, double route_length,
                     double curvature)
        : name(name), stops_on_route(stops_count), unique_stops(unique_stops), route_length_meters(route_length),
          curvature(curvature) {}