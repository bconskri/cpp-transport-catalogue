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
        ref.route_length = 0L;
        if (ref.stops.size() > 1) {
            for (size_t i = 0; i < ref.stops.size() - 1; ++i) {
                auto calc = ComputeDistance(ref.stops[i]->coords, ref.stops[i + 1]->coords);
                //std::cout << ref.stops[i]->name << " " << ref.stops[i + 1]->name << " distance " << calc <<std::endl;
                //std::cerr << std::setprecision(6)  << calc << std::endl;
                ref.route_length += calc;
                //std::cerr << std::setprecision(6)  << ref.route_length << std::endl;
            }
        }
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
                         ptr->route_length
    );
}