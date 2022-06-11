// напишите решение с нуля
// код сохраните в свой git-репозиторий
#include "transport_catalogue.h"

void TransportCatalogue::AddStop(Stop &&stop) {
    if (stopname_to_stop.count(stop.name) == 0) {
        auto &ref = stops_.emplace_back(std::move(stop));
        stopname_to_stop.insert({std::string_view(ref.name), &ref});
    }

}

void TransportCatalogue::AddRoute(BusRoute &&route) {
// Отсутствует ли этот маршрут route в базе?
//    if (all_buses_map_.count(route.route_name) == 0)
//    {
//        // Такого маршрута в базе нет, добавлям
//
//        // 2. Добавляем маршрут (автобус) в дек-хранилище, перемещая route
//        auto& ref = all_buses_data_.emplace_back(std::move(route));
//
//        // 3. Добавляем указатель на автобус (маршрут) из хранилища в словарь маршрутов
//        all_buses_map_.insert({ std::string_view(ref.route_name), &ref });
//
//        // 4. Подсчитываем уникальные остановки на маршруте
//        // Копируем вектор указателей
//        std::vector<StopPtr> tmp = ref.stops;
//        std::sort(tmp.begin(), tmp.end());
//        auto last = std::unique(tmp.begin(), tmp.end());
//        // Сохраняем количество уникальных остановок на маршруте
//        ref.unique_stops_qty = (last != tmp.end() ? std::distance(tmp.begin(), last) : tmp.size());
//
//        // 5. Если маршрут НЕ кольцевой, достраиваем обратный маршрут
//        if (!ref.is_circular)
//        {
//            // Достраиваем обратный маршрут для некольцевого маршрута
//            for (int i = ref.stops.size() - 2; i >= 0; --i)
//            {
//                // Добавляем в конец вектора остановки в обратном направлении, кроме конечной stops_list[size() - 1]
//                ref.stops.push_back(ref.stops[i]);
//            }
//        }
//
//        // 6. Подсчитываем длину маршрута
//        int stops_num = static_cast<int>(ref.stops.size());
//        if (stops_num > 1)
//        {
//            ref.geo_route_length = 0L;
//            ref.meters_route_length = 0U;
//            for (int i = 0; i < stops_num - 1; ++i)
//            {
//                ref.geo_route_length += ComputeDistance(ref.stops[i]->coords, ref.stops[i + 1]->coords);
//                ref.meters_route_length += GetDistance(ref.stops[i], ref.stops[i + 1]);
//            }
//            // Рассчитываем кривизну маршрута
//            ref.curvature = ref.meters_route_length / ref.geo_route_length;
//        }
//        else
//        {
//            // У маршрута 0 или 1 остановка. Длина == 0
//            ref.geo_route_length = 0L;
//            ref.meters_route_length = 0U;
//            ref.curvature = 1L;
//        }
//    }
}

const Stop *TransportCatalogue::GetStopByName(const std::string_view stop_name) const {
    if (stopname_to_stop.count(stop_name) == 0) {
        return nullptr;
    } else {
        return stopname_to_stop.at(stop_name);
    }
}

const BusRoute *TransportCatalogue::GetRouteByName(const std::string_view route_name) const {
    if (busname_to_bus.count(route_name) == 0) {
        return nullptr;
    } else {
        return busname_to_bus.at(route_name);
    }
}

const RouteInfo *TransportCatalogue::GetRouteInfo(const std::string_view route_name) const {
    const BusRoute *ptr = GetRouteByName(route_name);

    if (ptr == nullptr) {
        return nullptr;
    }
    return new RouteInfo(ptr->route_name,
                         ptr->stops.size(),
                         ptr->unique_stop_qty,
                         ptr->route_length
                         );
}