#include "transport_catalogue.h"
#include <iostream>
#include <algorithm>
#include <iomanip>

void TransportCatalogue::AddStop(Stop &&stop) {
    if (stopname_to_stop.count(stop.name) == 0) {
        auto &ref = stops_.emplace_back(std::move(stop));
        stopname_to_stop.insert({std::string_view(ref.name), &ref});
    }
}

void TransportCatalogue::AddRoute(BusRoute &&route) {

    if (busname_to_bus.count(route.name) == 0) {
        auto &ref = buses_.emplace_back(std::move(route));
        busname_to_bus.insert({std::string_view(ref.name), &ref});

        //calc uniq stops
        auto stops = ref.stops;
        //we need sort to remove duplicates
        std::sort(stops.begin(), stops.end());
        auto last = std::unique(stops.begin(), stops.end());
        ref.unique_stop_qty = (last != stops.end() ? std::distance(stops.begin(), last) : stops.size());

        //if it's not a circular route we need to build reverse path
        if (!ref.is_circular) {
            for (int i = ref.stops.size() - 2; i >= 0; --i) {
                ref.stops.emplace_back(ref.stops[i]);
            }
        }

        //calc route length
        ref.route_length_geo = 0L;
        ref.route_length_meters = 0L;
        if (ref.stops.size() > 1) {
            for (size_t i = 0; i < ref.stops.size() - 1; ++i) {
                auto from = ref.stops[i]->name;
                auto to = ref.stops[i + 1]->name;
                ref.route_length_geo += ComputeDistance(ref.stops[i]->coords, ref.stops[i + 1]->coords);
                ref.route_length_meters += GetDistance(ref.stops[i], ref.stops[i + 1]);
            }
            ref.curvature = ref.route_length_meters / ref.route_length_geo;
        }
    }
}

void TransportCatalogue::AddDistance(const Stop *stop_from, const Stop *stop_to, size_t dist) {
    if (stop_from != nullptr && stop_to != nullptr) {
        distances_to_stops_.emplace(std::make_pair(stop_from, stop_to), dist);
    }
}


const Stop *TransportCatalogue::GetStopByName(const std::string_view stop_name) const {
    if (stopname_to_stop.count(stop_name) == 0) {
        return nullptr;
    } else {
        return stopname_to_stop.at(stop_name);
    }
}

const BusRoute *TransportCatalogue::GetRouteByName(const std::string_view route_name) const {
    if (busname_to_bus.count(route_name) == 0) {
        return nullptr;
    } else {
        return busname_to_bus.at(route_name);
    }
}

const RouteInfo *TransportCatalogue::GetRouteInfo(const std::string_view route_name) const {
    const BusRoute *ptr = GetRouteByName(route_name);

    if (ptr == nullptr) {
        return nullptr;
    }
    return new RouteInfo(ptr->name,
                         ptr->stops.size(),
                         ptr->unique_stop_qty,
                         ptr->route_length_meters,
                         ptr->curvature
    );
}

std::optional<std::set<std::string_view>>
TransportCatalogue::GetBusesForStopInfo(const std::string_view stop_name) const {
    auto ptr = GetStopByName(stop_name);
    std::set<std::string_view> found_buses;
    if (ptr == nullptr) {
        return std::nullopt;
    }
    for (const auto &bus: busname_to_bus) {
        auto tmp = std::find_if(bus.second->stops.begin(), bus.second->stops.end(),
                                [stop_name](const Stop *stop) {
                                    return (stop->name == stop_name);
                                });
        if (tmp != bus.second->stops.end()) {
            found_buses.insert(bus.second->name);
        }
    }
    return found_buses;
}

double TransportCatalogue::GetDistance(const Stop *stop_from, const Stop *stop_to) {
    if (distances_to_stops_.count(std::pair{stop_from, stop_to}) > 0) {
        //auto tmp = distances_to_stops_.at(std::pair{stop_from, stop_to});
        return distances_to_stops_.at(std::pair{stop_from, stop_to});
    } else {
        // нет прямого расстояния - берем обратное
        if (distances_to_stops_.count(std::pair{stop_to, stop_from}) > 0) {
            //auto tmp = distances_to_stops_.at(std::pair{stop_to, stop_from});
            return distances_to_stops_.at(std::pair{stop_to, stop_from});
        } else {
            return 0U;
        }
    }
}