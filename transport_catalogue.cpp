// напишите решение с нуля
// код сохраните в свой git-репозиторий
#include "transport_catalogue.h"

void TransportCatalogue::AddStop(Stop&&) {};
void TransportCatalogue::AddRoute(BusRoute&&) {};

const Stop* TransportCatalogue::GetStopByName(const std::string_view) const {};
const BusRoute* TransportCatalogue::GetRouteByName(const std::string_view) const {};

const RouteInfo* TransportCatalogue::GetRouteInfo(const std::string_view) const {};