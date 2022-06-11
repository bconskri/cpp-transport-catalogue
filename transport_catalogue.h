// напишите решение с нуля
// код сохраните в свой git-репозиторий
#pragma once

#include "geo.h"
#include <unordered_map>
#include <deque>
#include <vector>

namespace transport_catalogue {
    class TransportCatalogue {
    public:
        TransportCatalogue() = default;

        struct Stop
        {
            Stop() = default;
            Stop(const std::string_view stop_name, const double lat, const double lng);
            Stop(const Stop* other_stop);

            std::string name;
            geo::Coordinates coords{ 0L,0L };
        };

        struct BusRoute
        {
            BusRoute() = default;
            BusRoute(const BusRoute* other_stop_ptr);

            std::string route_name;
            std::vector<const Stop*> stops;
            /* количество уникальных остановок, на которых останавливается автобус.
             * Одинаковыми считаются остановки, имеющие одинаковые названия.
             */
            size_t unique_stop_qty = 0U;
            /* длина маршрута в метрах.
             * В этом задании для простоты считается, что автобус проезжает путь между двумя
             * соседними остановками по кратчайшему расстоянию по земной поверхности.
             */
            double route_length = 0L;
            //cirlce or not route
            bool is_circular = false;
        };

        struct RouteInfo
        {
            std::string name;
            size_t stops_on_route = 0;      //R — количество остановок в маршруте автобуса от stop1 до stop1 включительно.
            size_t unique_stops = 0;        //количество уникальных остановок, на которых останавливается автобус.
            int64_t route_length = 0;       // длина маршрута в метрах.
        };


        void AddStop(Stop&&);
        void AddRoute(BusRoute&&);

        const Stop* GetStopByName(const std::string_view) const;
        const BusRoute* GetRouteByName(const std::string_view) const;

        const RouteInfo* GetRouteInfo(const std::string_view) const;

    private:
        std::deque<Stop> stops_;                                                    //all stops data
        std::unordered_map<std::string_view, const Stop*> stopname_to_stop;         //hash-table for search stop by name
        std::deque<BusRoute> buses_;
        std::unordered_map<std::string_view, const BusRoute*> busname_to_bus;       //hash-table for search bus by name

        // Hasher for std::pair<Stop*, Stop*>
        struct PairStopsHasher
        {
        public:
            std::size_t operator()(const std::pair<const Stop*, const Stop*> pair_of_stops) const noexcept {
                return 0; //FIXME
            }
        };

        std::unordered_map<std::pair<Stop*, Stop*>, int, PairStopsHasher> distances_to_other_stops;  //Hash stop to stop distance
    };
}