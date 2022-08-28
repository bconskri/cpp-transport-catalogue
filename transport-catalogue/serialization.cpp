#include "serialization.h"

namespace serialization {
    void serialization::Serializer::SetSettings(const SerializerSettings &settings) {
        settings_ = settings;
    }

    bool Serializer::FileDefined() const {
        return !settings_.file.empty();
    }

    void Serializer::Serialize(const BusRoute &route) {
        auto &route_ = *transport_package_.mutable_catalogue()->add_route();
        route_.set_name(route.name);
        route_.set_is_roundtrip(route.is_roundtrip);
        for (const auto& e : route.stops) {
            route_.add_stops(e);
        }
    }

    void Serializer::Serialize(const BusStop &stop) {
        auto &stop_ = *transport_package_.mutable_catalogue()->add_stops();
        stop_.set_name(stop.name);
        auto &coordinates = *stop_.mutable_coordinates();
        coordinates.set_lat(stop.coordinates.lat);
        coordinates.set_lng(stop.coordinates.lng);
        for (const auto& e : stop.distance_to_other_stops) {
            auto &distance = *stop_.add_distance_to_other_stops();
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
} //namespace serialization