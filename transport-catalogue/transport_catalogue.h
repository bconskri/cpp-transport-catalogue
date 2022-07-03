#pragma once

#include "domain.h"

#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <string_view>

class TransportCatalogue {
public:
    TransportCatalogue() = default;

    void AddStop(BusStop &&stop);

    void AddRoute(BusRoute &&route);

    void CalcRoutesStat();

    [[nodiscard]] BusStop *GetStopByName(std::string_view stop_name) const;

    [[nodiscard]] const BusRoute *GetRouteByName(std::string_view route_name) const;

    [[nodiscard]] const RouteInfo *GetRouteInfo(const std::string_view route_name) const;

    [[nodiscard]] std::optional<std::set<std::string_view>> GetBusesForStopInfo(const std::string_view stop_name) const;

    [[nodiscard]] double GetDistance(const BusStop *stop_from, const BusStop *stop_to);

    [[nodiscard]] const std::unordered_map<std::string_view, BusRoute *>& GetRoutes() const;
    [[nodiscard]] const std::unordered_map<std::string_view, BusStop *>& GetStops() const;

private:
    std::deque<BusStop> stops_;                                                //all stops data
    std::unordered_map<std::string_view, BusStop *> stopname_to_stop_;   //hash-table for search stop by name
    std::deque<BusRoute> buses_;
    std::unordered_map<std::string_view, BusRoute *> busname_to_bus_;          //hash-table for search bus by name
};