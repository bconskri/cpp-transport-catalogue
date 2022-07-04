#include "transport_catalogue.h"
#include <iostream>
#include <algorithm>
#include <iomanip>

void TransportCatalogue::AddStop(BusStop &&stop) {
    if (stopname_to_stop.count(stop.name) == 0) {
        auto &ref = stops_.emplace_back(std::move(stop));
        stopname_to_stop.insert({std::string_view(ref.name), &ref});
    }
}

void TransportCatalogue::AddRoute(BusRoute &&route) {
    if (busname_to_bus.count(route.name) == 0) {
        auto &ref = buses_.emplace_back(std::move(route));
        busname_to_bus.insert({std::string_view(ref.name), &ref});

        //if it's not a circular route we need to build reverse path
        if (!ref.is_circular) {
            for (int i = ref.stops.size() - 2; i >= 0; --i) {
                ref.stops.emplace_back(ref.stops[i]);
            }
        }
        //остальные поля не заполняем
        //после обработки всех остановок и маршрутов надо сделать пересчет статистикм
    }
}

void TransportCatalogue::CalcRoutesStat() {
    for (auto &[name, ptr]: busname_to_bus) {
        auto stops = ptr->stops;
        if (stops.empty())
            continue;

        ptr->route_length_geo = 0L;
        ptr->route_length_meters = 0L;
        ptr->curvature = 0;
        if (stops.size() > 1) {
            for (size_t i = 0; i < stops.size() - 1; ++i) {
                //calc route length
                auto from_stop = GetStopByName(ptr->stops[i]);
                auto to_stop = GetStopByName(ptr->stops[i + 1]);
                ptr->route_length_geo += ComputeDistance(from_stop->coords, to_stop->coords);
                ptr->route_length_meters += GetDistance(from_stop, to_stop);

                //transport_catalogue.AddBusOnStop(route_name, stops_[i]);
            }

            ptr->curvature = ptr->route_length_meters / ptr->route_length_geo;
        }
    }
}

const BusStop *TransportCatalogue::GetStopByName(const std::string_view stop_name) const {
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
                         ptr->unique_stops.size(),
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
        auto tmp = std::find(bus.second->stops.begin(), bus.second->stops.end(), stop_name);
        if (tmp != bus.second->stops.end()) {
            found_buses.insert(bus.second->name);
        }
    }
    return found_buses;
}

double TransportCatalogue::GetDistance(const BusStop *stop_from, const BusStop *stop_to) {
    if (stop_from->distance_to_other_stops.count(stop_to->name) > 0) {
        return stop_from->distance_to_other_stops.at(stop_to->name);
    } else {
        // нет прямого расстояния - берем обратное
        if (stop_to->distance_to_other_stops.count(stop_from->name) > 0) {
            return stop_to->distance_to_other_stops.at(stop_from->name);
        } else {
            return 0L;
        }
    }
}

RouteInfo::RouteInfo(std::string_view name, size_t stops_count, size_t unique_stops, double route_length,
                     double curvature)
        : name(name), stops_on_route(stops_count), unique_stops(unique_stops), route_length_meters(route_length),
          curvature(curvature) {

}