#pragma once

#include "transport_catalogue.pb.h"
#include "domain.h"
#include "transport_catalogue.h"

#include <string>
#include <fstream>

struct SerializerSettings {
    std::string file;
};

namespace serialization {
    class Serializer {
    private:
        SerializerSettings settings_;
        transport_catalogue::TransportPackage transport_package_;

    public:
        void SetSettings(const SerializerSettings &settings);
        bool FileDefined() const;
        void Serialize(const BusRoute& route);
        void Serialize(const BusStop& stop);
        void FlushToFile();
        void Deserialize(std::shared_ptr<TransportCatalogue> transport_catalogue);
    };
} //namespace serialization