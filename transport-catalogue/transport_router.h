#pragma once

#include "transport_catalogue.h"
#include "json_builder.h"
#include "graph.h"
#include "router.h"

#include <string>
#include <vector>
#include <memory>

struct RoutingSettings {
    double bus_wait_time;
    double bus_velocity;
};

enum RouteSegmentType {
    Wait,
    Bus,
};
struct RouteEdgeInfo {
    std::string stop_name;
    std::string bus;
    int span_count;
    double time;
    RouteSegmentType type;

};

//forward declaration
namespace serialization {
    class Serializer;
}

namespace transport_router {

    class RouteManager {
    public:
        using Edge = graph::Edge<double>;
        using Router = graph::Router<double>;

        RouteManager() = default;

        void SetSettings(const RoutingSettings &);

        std::optional<std::pair<double, std::vector<RouteEdgeInfo>>>
        BuildRoute(std::shared_ptr<TransportCatalogue>, std::string, std::string);

    private:
        //we need it friend to access private fileds to serialize
        friend class serialization::Serializer;
        //
        RoutingSettings settings_;
        //VertexId надо где-то хранить, так как библиотека Graph работает только с ребрами
        std::unordered_map<std::string, graph::VertexId> stop_to_vertex_;
        //маршрутизатор возвращает нам номера ребер графа - по которым проложен маршрут
        //нужно уметь восстановить по номеру ребра все данные для вывода в отчет
        std::unordered_map<graph::EdgeId, RouteEdgeInfo> edge_to_route_segment_;
        //Graph надо строить 1 раз. Сделаем LazyValue механизм
        std::optional<graph::DirectedWeightedGraph<double>> graph_ = std::nullopt;
        //здесь будем хранить роутер
        std::shared_ptr<graph::Router<double>> router_ = nullptr;
        //
        void BuildBusRoutesGraph(std::shared_ptr<TransportCatalogue>);
    };
}