#include "request_handler.h"

void RequestHandler::ProcessRequest(const json::Node& requests_) const
{
    json::Array result;
    for (auto& request : requests_.AsArray()) {
        const auto& request_map = request.AsMap();
        const auto& type = request_map.at("type").AsString();

        json::Node dict;
        if (type == "Stop") {
            const std::string& stop_name = request_map.at("name").AsString();
            const auto id = request_map.at("id").AsInt();

            if (!db_.StopInfo(stop_name)) {
                dict = json::Builder{}.StartDict()
                    .Key("request_id").Value(id)
                    .Key("error_message").Value("not found")
                    .EndDict()
                    .Build().AsMap();
            }
            else {
                json::Array buses;
                auto& get_buses_by_stop = GetBusesByStop(stop_name);
                for (auto& bus : get_buses_by_stop) {
                    buses.push_back(bus);
                }
                dict = json::Builder{}.StartDict()
                    .Key("request_id").Value(id)
                    .Key("buses").Value(buses)
                    .EndDict()
                    .Build().AsMap();            
            }
            result.push_back(dict);
        }
        if (type == "Bus") {
            const std::string& route_number = request_map.at("name").AsString();
            const auto id = request_map.at("id").AsInt();
            if (!db_.BusInfo(route_number)) {
                dict = json::Builder{}.StartDict()
                    .Key("request_id").Value(id)
                    .Key("error_message").Value("not found")
                    .EndDict()
                    .Build().AsMap();
            }
            else {
                auto bus_stat = db_.GetBusStat(route_number);
                dict = json::Builder{}.StartDict()
                    .Key("request_id").Value(id)
                    .Key("curvature").Value(bus_stat->curvature)
                    .Key("route_length").Value(bus_stat->length)
                    .Key("stop_count").Value(static_cast<int>(bus_stat->stops))
                    .Key("unique_stop_count").Value(static_cast<int>(bus_stat->unique_stops))
                    .EndDict()
                    .Build().AsMap();
            }
            result.push_back(dict);
        }
        if (type == "Map") {
            const auto id = request_map.at("id").AsInt();
            std::ostringstream strm;
            svg::Document map = RenderMap();
            map.Render(strm);
            dict = json::Builder{}.StartDict()
                .Key("request_id").Value(id)
                .Key("map").Value(strm.str())
                .EndDict()
                .Build();
            result.push_back(dict);
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
