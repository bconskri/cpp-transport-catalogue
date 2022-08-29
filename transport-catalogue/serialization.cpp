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

    void Serializer::Deserialize(std::shared_ptr<TransportCatalogue> transport_catalogue) {
        if (FileDefined()) {
            std::fstream ifs(settings_.file, std::ios::binary | std::ios::in);
            if (ifs.is_open()) {
                transport_package_.Clear();
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
                }
            }
        }
    }
} //namespace serialization