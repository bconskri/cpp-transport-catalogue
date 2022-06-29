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