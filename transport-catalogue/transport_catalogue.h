#pragma once

#include "geo.h"
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <vector>
#include <string_view>
#include <string>
#include <set>
#include <functional>

struct BusStop {

    std::string name;
    geo::Coordinates coords{0L, 0L};
    std::unordered_map<std::string, unsigned> distance_to_other_stops;
};

struct BusRoute {

    std::string name;
    //std::vector<const BusStop *> stops;
    std::vector<std::string> stops;
    std::unordered_set<std::string> unique_stops;
    /* длина маршрута в метрах.
     * В этом задании для простоты считается, что автобус проезжает путь между двумя
     * соседними остановками по кратчайшему расстоянию по земной поверхности.
     */
    double route_length_geo = 0L;
    //length of route in meters by roads
    double route_length_meters = 0L;
    //извилистость, то есть отношение фактической длины маршрута к географическому расстоянию
    double curvature = 0L;
    //circle or not route
    bool is_circular = false;
};

struct RouteInfo {
    explicit RouteInfo(std::string_view name, size_t stops_count, size_t unique_stops, double route_length,
                       double curvature);

    std::string name;
    size_t stops_on_route = 0U;      //R — количество остановок в маршруте автобуса от stop1 до stop1 включительно.
    size_t unique_stops = 0U;        //количество уникальных остановок, на которых останавливается автобус.
    double route_length_meters = 0L;       // длина маршрута в метрах.
    double curvature = 0L;          //извилистость, то есть отношение фактической длины маршрута к географическому расстоянию
};

class TransportCatalogue {
public:
    TransportCatalogue() = default;

    void AddStop(BusStop &&stop);

    void AddRoute(BusRoute &&route);

    void CalcRoutesStat();

    [[nodiscard]] const BusStop *GetStopByName(std::string_view stop_name) const;

    [[nodiscard]] const BusRoute *GetRouteByName(std::string_view route_name) const;

    [[nodiscard]] const RouteInfo *GetRouteInfo(const std::string_view route_name) const;

    [[nodiscard]] std::optional<std::set<std::string_view>> GetBusesForStopInfo(const std::string_view stop_name) const;

    [[nodiscard]] double GetDistance(const BusStop *stop_from, const BusStop *stop_to);

private:
    std::deque<BusStop> stops_;                                                    //all stops data
    std::unordered_map<std::string_view, const BusStop *> stopname_to_stop;         //hash-table for search stop by name
    std::deque<BusRoute> buses_;
    std::unordered_map<std::string_view, BusRoute *> busname_to_bus;       //hash-table for search bus by name
};