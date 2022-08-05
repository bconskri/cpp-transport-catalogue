#include "transport_router.h"

namespace transport_router {
    void transport_router::RouteManager::SetSettings(const RoutingSettings &settings) {
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
            if (graph_->GetEdgeCount() > 0) {
                router_ = std::make_unique<Router>(Router(graph_.value()));
            }
        }
        //в графе нет ни одного ребра - маршруты строить просто не на чем
        if (graph_->GetEdgeCount() == 0) {
            return std::nullopt;
        }
        //
        std::vector<RouteEdgeInfo> route_segments;
        if (from == to) { //крайний случай - быстрый возврат пустой не nullopt
            return std::pair{0, route_segments};
        }
        //остановка входит в stop_to_vertex_, только если она была хоть в одном маршруте
        //только тогода есть смысл строить до нее маршрут
        if (stop_to_vertex_.count(from) > 0 && stop_to_vertex_.count(to) > 0) {
            auto route_build = router_->BuildRoute(stop_to_vertex_.at(from),
                                                   stop_to_vertex_.at(to));
            if (route_build) {
                if (route_build.value().edges.size() == 0 && route_build.value().weight == 0) {
                    return std::nullopt;
                }
                for (const auto &edge_id: route_build.value().edges) {
                    route_segments.push_back(edge_to_route_segment_.at(edge_id));
                }
                return std::pair{route_build.value().weight, route_segments};
            }
        }
        return std::nullopt;
    }

    void RouteManager::BuildBusRoutesGraph(std::shared_ptr<TransportCatalogue> transport_catalogue) {
        using namespace std::literals;
        //создаем пустой граф с количеством вертексов = кол-ву остановок *2
        // так как на остановке в месте посадки добавляется ожидание
        graph_ = graph::DirectedWeightedGraph<double>(2.0 * transport_catalogue->GetStopsCount());
        //переберем все маршруты, заполним Vertexes
        for (const auto &route: transport_catalogue->GetRoutes()) {
            for (const auto &bus_stop: route.second->stops) {
                //добавляем вершину
                if (stop_to_vertex_.count(bus_stop) == 0) {
                    stop_to_vertex_[bus_stop] = stop_to_vertex_.size();
                }
                //добавляем вертекс - ожидание, вдруг здесь будет куда пересаживаться
                if (stop_to_vertex_.count(bus_stop + "_wait"s) == 0) {
                    stop_to_vertex_[bus_stop + "_wait"s] = stop_to_vertex_.size();
                    //вертекс ожидания добавили - сделаем к нему ребро из остановки
                    auto edge_id = graph_->AddEdge(Edge{stop_to_vertex_.at(bus_stop),
                                                        stop_to_vertex_.at(bus_stop + "_wait"s),
                                                        settings_.bus_wait_time});
                    edge_to_route_segment_[edge_id] = RouteEdgeInfo{bus_stop, ""s, 0, settings_.bus_wait_time,
                                                                    RouteSegmentType::Wait};
                }
            }
            //все вертексы остановок и ожидания для данного маршрута добавлены
            //теперь создадим ребра от каждой остановки к каждой в маршруте
            for (auto it_start = route.second->stops.begin(); it_start != route.second->stops.end(); ++it_start) {
                //если это не круговой маршрут - он хранится достроенным в обратную сторону
                //нам сейчас надо останавливаться на конечной
                //if (!route.second->is_roundtrip && *it_start == route.second->end_stop) {
                if (!route.second->is_roundtrip &&
                    std::distance(it_start, route.second->stops.begin()) == long(route.second->end_stop)) {
                    break;
                }
                double time_to_drive_forward = 0;
                double time_to_drive_reverse = 0;
                int span_count = 0;
                std::string prev_stop = *it_start;
                for (auto it_end = std::next(it_start); it_end != route.second->stops.end(); ++it_end) {
                    time_to_drive_forward += transport_catalogue->GetDistance(prev_stop, *it_end)
                                             / settings_.bus_velocity;
                    time_to_drive_reverse += transport_catalogue->GetDistance(*it_end, prev_stop)
                                             / settings_.bus_velocity;
                    //
                    auto edge_id = graph_->AddEdge(Edge{stop_to_vertex_.at(*it_start + "_wait"s),
                                                        stop_to_vertex_.at(*it_end),
                                                        time_to_drive_forward});
                    edge_to_route_segment_[edge_id] = RouteEdgeInfo{*it_start, std::string(route.first),
                                                                    ++span_count, time_to_drive_forward,
                                                                    RouteSegmentType::Bus};
                    //если маршрут некольцевой
                    // сразу добавим обратное ребро, так как у нас аккумулировано расстояние
                    // и не надо будет его считать второй раз
                    if (!route.second->is_roundtrip) {
                        edge_id = graph_->AddEdge(Edge{stop_to_vertex_.at(*it_end + "_wait"s),
                                                       stop_to_vertex_.at(*it_start),
                                                       time_to_drive_reverse});
                        edge_to_route_segment_[edge_id] = RouteEdgeInfo{*it_end, std::string(route.first),
                                                                        span_count, time_to_drive_reverse,
                                                                        RouteSegmentType::Bus};
                    }
                    //
                    prev_stop = *it_end;
                    //если это не круговой маршрут - он хранится достроенным в обратную сторону
                    //нам сейчас надо останавливаться на конечной. До нее дошли
                    //if (!route.second->is_roundtrip && *it_end == route.second->end_stop) {
                    if (!route.second->is_roundtrip &&
                        std::distance(it_end, route.second->stops.begin()) == long(route.second->end_stop)) {
                        break;
                    }
                }
            }
        }
    }
}