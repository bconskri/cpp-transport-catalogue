#pragma once

#include "geo.h"
#include <unordered_map>
#include <deque>
#include <vector>
#include <string_view>
#include <string>
#include <set>


struct Stop {
    Stop() = default;

    Stop(const std::string_view stop_name, const double lat, const double lng)
            : name(stop_name) {
        coords.lat = lat;
        coords.lng = lng;
    }
    //Stop(const Stop* other_stop);

    std::string name;
    geo::Coordinates coords{0L, 0L};
};

struct BusRoute {
    BusRoute() = default;
    //BusRoute(const BusRoute* other_stop_ptr);

    std::string name;
    std::vector<const Stop *> stops;
    /* количество уникальных остановок, на которых останавливается автобус.
     * Одинаковыми считаются остановки, имеющие одинаковые названия.
     */
    size_t unique_stop_qty = 0U;
    /* длина маршрута в метрах.
     * В этом задании для простоты считается, что автобус проезжает путь между двумя
     * соседними остановками по кратчайшему расстоянию по земной поверхности.
     */
    double route_length_geo = 0L;
    //length of route in meters by roads
    size_t route_length_meters = 0U;
    //извилистость, то есть отношение фактической длины маршрута к географическому расстоянию
    double curvature = 0L;
    //circle or not route
    bool is_circular = false;
};

struct RouteInfo {
    explicit RouteInfo(std::string_view name, size_t stops_count, size_t unique_stops, int64_t route_length,
                       double curvature)
            : name(name), stops_on_route(stops_count), unique_stops(unique_stops), route_length_meters(route_length),
              curvature(curvature) {

    }

    std::string name;
    size_t stops_on_route = 0U;      //R — количество остановок в маршруте автобуса от stop1 до stop1 включительно.
    size_t unique_stops = 0U;        //количество уникальных остановок, на которых останавливается автобус.
    int64_t route_length_meters = 0L;       // длина маршрута в метрах.
    double curvature = 0L;          //извилистость, то есть отношение фактической длины маршрута к географическому расстоянию
};

class TransportCatalogue {
public:
    TransportCatalogue() = default;

    void AddStop(Stop &&stop);

    void AddRoute(BusRoute &&route);

    void AddDistance(const Stop *stop_from, const Stop *stop_to, size_t dist);

    [[nodiscard]] const Stop *GetStopByName(std::string_view stop_name) const;

    [[nodiscard]] const BusRoute *GetRouteByName(std::string_view route_name) const;

    [[nodiscard]] const RouteInfo *GetRouteInfo(const std::string_view route_name) const;

    [[nodiscard]] std::optional<std::set<std::string_view>> GetBusesForStopInfo(const std::string_view stop_name) const;

    [[nodiscard]] size_t GetDistance(const Stop *stop_from, const Stop *stop_to);

    struct PairStopsHasher {
    public:
        std::size_t
        operator()([[maybe_unused]] const std::pair<const Stop *, const Stop *> pair_of_stops) const noexcept {
            //return std::size_t(pair_of_stops.first*37 + pair_of_stops.second);
            return 0; //FIXME
        }
    };

    std::unordered_map<std::pair<const Stop *, const Stop *>, int, PairStopsHasher> distances_to_stops_;  //Hash stop to stop distance

private:
    std::deque<Stop> stops_;                                                    //all stops data
    std::unordered_map<std::string_view, const Stop *> stopname_to_stop;         //hash-table for search stop by name
    std::deque<BusRoute> buses_;
    std::unordered_map<std::string_view, const BusRoute *> busname_to_bus;       //hash-table for search bus by name

    // Hasher for std::pair<Stop*, Stop*>
//    struct PairStopsHasher {
//    public:
//        std::size_t
//        operator()([[maybe_unused]] const std::pair<const Stop *, const Stop *> pair_of_stops) const noexcept {
//            //return std::size_t(pair_of_stops.first*37 + pair_of_stops.second);
//            return 0; //FIXME
//        }
//    };

//    std::unordered_map<std::pair<const Stop *, const Stop *>, int, PairStopsHasher> distances_to_stops;  //Hash stop to stop distance
};