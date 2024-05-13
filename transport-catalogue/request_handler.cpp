/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

#include "request_handler.h"

std::optional<Information> RequestHandler::GetBusStat(const std::string_view& bus_name) const
{
    Information information{};

    auto bus_info = db_.BusInfo(bus_name);
    if (!bus_info) throw std::invalid_argument("bus not found");

    information.name = bus_info->bus_name;
    auto& stops = bus_info->stops;

    if (!bus_info->is_roundtrip) information.stops = (stops.size() * 2) - 1;
    else information.stops = stops.size();
    double geo = 0.0;
    double fact = 0.0;
    if (bus_info->is_roundtrip) {
        for (size_t n = 0; n < stops.size() - 1; n++) {
            fact += db_.GetDistance(stops[n], stops[n + 1]);
            geo += geo::ComputeDistance(db_.StopInfo(stops[n])->coord, db_.StopInfo(stops[n + 1])->coord);
        }
    }
    else {
        for (size_t n = 0; n < stops.size() - 1; n++) {
            fact += db_.GetDistance(stops[n], stops[n + 1]) + db_.GetDistance(stops[n + 1], stops[n]);
            geo += geo::ComputeDistance(db_.StopInfo(stops[n])->coord, db_.StopInfo(stops[n + 1])->coord) * 2;

        }
    }
    information.length = fact;
    information.curvature = fact / geo;
    std::unordered_set<std::string_view> unique_stops;
    for (const auto& s : stops) {
        unique_stops.insert(s);
    }
    information.unique_stops = unique_stops.size();

    return information;
}


const std::set<std::string> RequestHandler::GetBusesByStop(const std::string_view& stop_name) const
{
    return db_.StopInfo(stop_name)->buses_by_stop;
}

svg::Document RequestHandler::RenderMap() const
{
    return svg::Document(renderer_.GetRoadMap(db_.GetBusIndex(), db_.GetStopIndex()));
}
