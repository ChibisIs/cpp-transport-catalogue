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
        if (type == "Route") {
            const auto id = request_map.at("id").AsInt();

            const std::string_view stop_from = request_map.at("from").AsString();
            const std::string_view stop_to = request_map.at("to").AsString();
            const auto& route_info = router_.FindRoute(stop_from, stop_to);

            if (!route_info) {
                result.push_back(json::Builder{}
                    .StartDict()
                    .Key("request_id").Value(id)
                    .Key("error_message").Value("not found")
                    .EndDict()
                    .Build());
            }
            else {
                json::Array items;
                double total_time = 0.0;
                items.reserve(route_info.value().edges.size());
                const auto& edges = route_info.value().edges;

                for (auto& edge_id : edges) {
                    
                    const graph::Edge<double> edge = router_.GetGraphEdge(edge_id);

                    if (edge.quality == 0) {
                        items.emplace_back(json::Node(json::Builder{}
                            .StartDict()
                            .Key("stop_name").Value(edge.name)
                            .Key("time").Value(edge.weight)
                            .Key("type").Value("Wait")
                            .EndDict()
                            .Build()));

                        total_time += edge.weight;
                    }
                    else {
                        items.emplace_back(json::Node(json::Builder{}
                            .StartDict()
                            .Key("bus").Value(edge.name)
                            .Key("span_count").Value(static_cast<int>(edge.quality))
                            .Key("time").Value(edge.weight)
                            .Key("type").Value("Bus")
                            .EndDict()
                            .Build()));

                        total_time += edge.weight;
                    }
                }

                result.push_back(json::Builder{}
                    .StartDict()
                    .Key("request_id").Value(id)
                    .Key("total_time").Value(total_time)
                    .Key("items").Value(items)
                    .EndDict()
                    .Build());
            }
            
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
