#include "request_handler.h"

void RequestHandler::ProcessRequest(const json::Node& requests_) const
{
    json::Array result;
    for (auto& request : requests_.AsArray()) {
        const auto& request_map = request.AsMap();
        const auto& type = request_map.at("type").AsString();
        json::Dict dict;
        if (type == "Stop") {
            const std::string& stop_name = request_map.at("name").AsString();
            dict["request_id"] = request_map.at("id").AsInt();
            if (!db_.StopInfo(stop_name)) {
                dict["error_message"] = json::Node{ static_cast<std::string>("not found") };
            }
            else {
                json::Array buses;
                auto& get_buses_by_stop = GetBusesByStop(stop_name);
                for (auto& bus : get_buses_by_stop) {
                    buses.push_back(bus);
                }
                dict["buses"] = buses;
            }
            result.emplace_back(dict);
        }
        if (type == "Bus") {
            const std::string& route_number = request_map.at("name").AsString();
            dict["request_id"] = request_map.at("id").AsInt();
            if (!db_.BusInfo(route_number)) {
                dict["error_message"] = json::Node{ static_cast<std::string>("not found") };
            }
            else {
                auto bus_stat = db_.GetBusStat(route_number);
                dict["curvature"] = bus_stat->curvature;
                dict["route_length"] = bus_stat->length;
                dict["stop_count"] = static_cast<int>(bus_stat->stops);
                dict["unique_stop_count"] = static_cast<int>(bus_stat->unique_stops);
            }
            result.emplace_back(dict);
        }
        if (type == "Map") {
            dict["request_id"] = request_map.at("id").AsInt();
            std::ostringstream strm;
            svg::Document map = RenderMap();
            map.Render(strm);
            dict["map"] = strm.str();
            result.emplace_back(dict);
        }
    }
    json::Print(json::Document{ result }, std::cout);
}

const std::set<std::string> RequestHandler::GetBusesByStop(const std::string_view& stop_name) const
{
    return db_.StopInfo(stop_name)->buses_by_stop;
}

svg::Document RequestHandler::RenderMap() const
{
    return svg::Document(renderer_.GetRoadMap(db_.GetBusIndex(), db_.GetStopIndex()));
}
