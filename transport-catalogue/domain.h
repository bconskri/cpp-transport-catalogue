#pragma once

#include "geo.h"

#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области (domain)
 * вашего приложения и не зависят от транспортного справочника. Например Автобусные маршруты и Остановки.
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */

struct BusStop {

    std::string name;
    geo::Coordinates coordinates{0L, 0L};
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
    bool is_roundtrip = false;
};

struct RouteInfo {
    explicit RouteInfo(std::string_view name, size_t stops_count, size_t unique_stops, double route_length,
                       double curvature)
            : name(name), stops_on_route(stops_count), unique_stops(unique_stops), route_length_meters(route_length),
              curvature(curvature) {

    }

    std::string name;
    size_t stops_on_route = 0U;      //R — количество остановок в маршруте автобуса от stop1 до stop1 включительно.
    size_t unique_stops = 0U;        //количество уникальных остановок, на которых останавливается автобус.
    double route_length_meters = 0L;       // длина маршрута в метрах.
    double curvature = 0L;          //извилистость, то есть отношение фактической длины маршрута к географическому расстоянию
};