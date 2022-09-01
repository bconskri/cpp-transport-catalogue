#pragma once

#include "transport_catalogue.pb.h"
#include "domain.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"
#include "svg.h"

#include <string>
#include <fstream>
#include <variant>

struct SerializerSettings {
    std::string file;
};

namespace serialization {
    class Serializer {
    private:
        SerializerSettings settings_;
        transport_package::TransportPackage transport_package_;

    public:
        Serializer() = default;

        void SetSettings(const SerializerSettings &settings);
        bool FileDefined() const;
        void Serialize(const BusRoute& route);
        void Serialize(const BusStop& stop);
        void Serialize(const RenderSettings& settings);
        void Serialize(const RoutingSettings & settings);
        void Serialize(std::shared_ptr<TransportCatalogue> transport_catalogue,
                       std::shared_ptr<transport_router::RouteManager> route_manager);
        void FlushToFile();
        void Deserialize(std::shared_ptr<TransportCatalogue> transport_catalogue,
                         std::shared_ptr<transport_router::RouteManager> route_manager,
                         std::shared_ptr<map_renderer::MapRender> map_render);
    };
} //namespace serialization