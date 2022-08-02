#include "route_manager.h"

namespace route_manager {
    void route_manager::RouteManager::SetSettings(const RoutingSettings &settings) {
        settings_ = settings;
    }

    std::optional<std::pair<double, std::vector<RouteEdgeInfo>>>
    RouteManager::BuildRoute(std::shared_ptr<TransportCatalogue> transport_catalogue, std::string from,
                             std::string to) {
        //если граф ни разу не заполнялся - это первый запрос маршрута
        //lazy value
        if (!graph_) {
            //заполним сначала граф
            BuildBusRoutesGraph(transport_catalogue);
            //создадим на нем роутер
            router_ = std::make_unique<Router>(Router(graph_.value()));
        }
        //
        auto route_build = router_->BuildRoute(stop_to_vertex_[from], stop_to_vertex_[to]);
        if (route_build) {
            std::vector<RouteEdgeInfo> route_segments;
            for (const auto &edge_id: route_build.value().edges) {
                route_segments.push_back(edge_to_route_segment_[edge_id]);
            }
            return std::pair{route_build.value().weight, route_segments};
        }
        return std::nullopt;
    }

    void RouteManager::BuildBusRoutesGraph(std::shared_ptr<TransportCatalogue> transport_catalogue) {
        using namespace std::literals;
        //создаем пустой граф с количеством вертексов = кол-ву остановок *2
        // в месте посадки добавляется ожидание
        graph_ = graph::DirectedWeightedGraph<double>(2.0 * transport_catalogue->GetStopsCount());
        //переберем все маршруты, заполним Vertexes
        for (const auto &route: transport_catalogue->GetRoutes()) {
            for (const auto &bus_stop: route.second->stops) {
                //если маршрут линейный, он хранится достроенным в обратную сторону
                // надо останавливаться на последней остановке

                //добавляем вершину
                if (stop_to_vertex_.count(bus_stop) == 0) {
                    //vertex_to_stop_[stop_to_vertex_.size()] = bus_stop;
                    stop_to_vertex_[bus_stop] = stop_to_vertex_.size();

                }
                //добавляем спутник - ожидание, вдруг здесь будет куда пересаживаться
                if (stop_to_vertex_.count(bus_stop + "_wait"s) == 0) {
                    stop_to_vertex_[bus_stop + "_wait"s] = stop_to_vertex_.size();
                    //вертекс ожидания добавили - сделаем к нему ребро из остановки
                    auto edge_id = graph_->AddEdge(Edge{stop_to_vertex_[bus_stop],
                                                        stop_to_vertex_[bus_stop + "_wait"s],
                                                        settings_.bus_wait_time});
                    edge_to_route_segment_[edge_id] = RouteEdgeInfo{.bus = ""s,
                            .type = RouteSegmentType::Wait,
                            .span_count = 0,
                            .stop_name = bus_stop,
                            .time = settings_.bus_wait_time};
                }
            }
            //все вертексы остановок и ожидания для данного маршрута добавлены
            //теперь создадим ребра от каждой остановки к каждой в маршруте
            for (auto it_start = route.second->stops.begin(); it_start != route.second->stops.end(); it_start++) {
                //если это не круговой маршрут - он хранится достроенным в обратную сторону
                //нам сейчас надо останавливаться на конечной
                if (!route.second->is_roundtrip && *it_start == route.second->end_stop) {
                    break;
                }
                double time_to_drive = 0;
                int span_count = 0;
                std::string prev_stop = *it_start;
                for (auto it_end = std::next(it_start); it_end != route.second->stops.end(); it_end++) {
                    time_to_drive += transport_catalogue->GetDistance(prev_stop, *it_end)
                                     / settings_.bus_velocity;
                    //
                    auto edge_id = graph_->AddEdge(
                            Edge{stop_to_vertex_[*it_start + "_wait"s], stop_to_vertex_[*it_end], time_to_drive});
                    edge_to_route_segment_[edge_id] = RouteEdgeInfo{
                            .bus = std::string(route.first),
                            .type = RouteSegmentType::Bus,
                            .span_count = ++span_count,
                            .stop_name = *it_start,
                            .time = time_to_drive};
                    //если маршрут некольцевой
                    // сразу добавим обратное ребро, так как у нас аккумулировано расстояние
                    // и не надо будет его считать второй раз
                    if (!route.second->is_roundtrip) {
                        edge_id = graph_->AddEdge(Edge{stop_to_vertex_[*it_end + "_wait"s], stop_to_vertex_[*it_start],
                                                       time_to_drive});
                        edge_to_route_segment_[edge_id] = RouteEdgeInfo{
                                .bus = std::string(route.first),
                                .type = RouteSegmentType::Bus,
                                .span_count = span_count,
                                .stop_name = *it_end,
                                .time = time_to_drive};
                    }
                    //
                    prev_stop = *it_end;
                    //если это не круговой маршрут - он хранится достроенным в обратную сторону
                    //нам сейчас надо останавливаться на конечной
                    if (!route.second->is_roundtrip && *it_end == route.second->end_stop) {
                        break;
                    }
                }
            }
        }
    }
}