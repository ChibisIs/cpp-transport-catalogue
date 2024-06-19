#include "json_reader.h"

const json::Node& JsonReader::GetBaseRequests() const {
    return input_.GetRoot().AsMap().at("base_requests");
}

const json::Node& JsonReader::GetRenderSettings() const
{
    return input_.GetRoot().AsMap().at("render_settings");
}

const json::Node& JsonReader::GetStatRequests() const {
    return input_.GetRoot().AsMap().at("stat_requests");
}

const json::Node& JsonReader::GetRoutingSettings() const
{
    return input_.GetRoot().AsMap().at("routing_settings");
}

void JsonReader::FillCatalogue(catalogue::TransportCatalogue& catalogue) {
    const json::Array& arr = GetBaseRequests().AsArray();
    for (auto& request_stops : arr) {
        const auto& request_stops_map = request_stops.AsMap();
        const auto& type = request_stops_map.at("type").AsString();
        if (type == "Stop") {
            auto [name, coord, stop_distances, _] = FillStop(request_stops_map);
            catalogue.AddStop(name, coord);
        }
    }
    for (auto& request_stops : arr) {
        const auto& request_stops_map = request_stops.AsMap();
        const auto& type = request_stops_map.at("type").AsString();
        if (type == "Stop") {
            auto [name, coord, stop_distances, _] = FillStop(request_stops_map);
            for (auto& [to_name, dist] : stop_distances) {
                catalogue.AddDistance(name, to_name, dist);
            }
        }
    }

    for (auto& request_bus : arr) {
        const auto& request_bus_map = request_bus.AsMap();
        const auto& type = request_bus_map.at("type").AsString();
        if (type == "Bus") {
            auto [name, stops, is_roundtrip] = FillBus(request_bus_map);
            catalogue.AddBus(std::string(name), stops, is_roundtrip);
        }
    }
}

renderer::MapRenderer JsonReader::FillRenderSettings(const json::Dict& request_map) const
{
    RenderSettings settings = {};
    settings.width = request_map.at("width").AsDouble();
    settings.height = request_map.at("height").AsDouble();
    settings.padding = request_map.at("padding").AsDouble();
    settings.line_width = request_map.at("line_width").AsDouble();
    settings.stop_radius = request_map.at("stop_radius").AsDouble();
    settings.bus_label_font_size = request_map.at("bus_label_font_size").AsInt();
    auto& bus_label_offset_ = request_map.at("bus_label_offset").AsArray();
    settings.bus_label_offset = { bus_label_offset_[0].AsDouble(), bus_label_offset_[1].AsDouble() };
    settings.stop_label_font_size = request_map.at("stop_label_font_size").AsInt();
    auto& stop_label_offset_ = request_map.at("stop_label_offset").AsArray();
    settings.stop_label_offset = { stop_label_offset_[0].AsDouble(), stop_label_offset_[1].AsDouble() };

    if (request_map.at("underlayer_color").IsString()) settings.underlayer_color = request_map.at("underlayer_color").AsString();
    else if (request_map.at("underlayer_color").IsArray()) {
        auto& underlayer_color = request_map.at("underlayer_color").AsArray();
        if (underlayer_color.size() == 3) {
            settings.underlayer_color = svg::Rgb(underlayer_color[0].AsInt(), underlayer_color[1].AsInt(), underlayer_color[2].AsInt());
        }
        else if (underlayer_color.size() == 4) {
            settings.underlayer_color = svg::Rgba(underlayer_color[0].AsInt(), underlayer_color[1].AsInt(), underlayer_color[2].AsInt(), underlayer_color[3].AsDouble());
        }
        else throw std::logic_error("Wrong underlayer color!");
    }
    else throw std::logic_error("Wrong underlayer color!");
    settings.underlayer_width = request_map.at("underlayer_width").AsDouble();
    settings.color_palette = {};
    auto& color_palette_ = request_map.at("color_palette").AsArray();
    for (auto& elem : color_palette_) {
        if (elem.IsString()) {
            settings.color_palette.emplace_back(elem.AsString());
        }
        else if (elem.IsArray()) {
            auto& color_type = elem.AsArray();
            if (color_type.size() == 3) {
                settings.color_palette.emplace_back(svg::Rgb(color_type[0].AsInt(), color_type[1].AsInt(), color_type[2].AsInt()));
            }
            else if (color_type.size() == 4) {
                settings.color_palette.emplace_back(svg::Rgba(color_type[0].AsInt(), color_type[1].AsInt(), color_type[2].AsInt(), color_type[3].AsDouble()));
            }
            else throw std::logic_error("Wrong color palette!");
        }
        else throw std::logic_error("Wrong color palette!");
    }
    return settings;
}

catalogue::RouteSettings JsonReader::FillRoutingSettings(const json::Node& settings) const
{
    catalogue::RouteSettings routing_settings;
    routing_settings.bus_wait_time_ = settings.AsMap().at("bus_wait_time").AsInt();
    routing_settings.bus_velocity_ = settings.AsMap().at("bus_velocity").AsDouble();
    return routing_settings;
}

catalogue::Stop JsonReader::FillStop(const json::Dict& request_map) const
{
    auto& name = request_map.at("name").AsString();
    geo::Coordinates coord = { request_map.at("latitude").AsDouble(), request_map.at("longitude").AsDouble() };

    std::map<std::string_view, int> road_distances;
    auto& stops = request_map.at("road_distances").AsMap();
    for (auto& [stop, dist] : stops) {
        road_distances.emplace(stop, dist.AsInt());
    }
    return { name, coord, road_distances, {} };
}

catalogue::Bus JsonReader::FillBus(const json::Dict& request_map) const
{
    auto& name = request_map.at("name").AsString();
    std::vector<std::string> stops;
    for (auto& stop : request_map.at("stops").AsArray()) {
        stops.push_back(stop.AsString());
    }
    bool is_roundtrip = request_map.at("is_roundtrip").AsBool();
    return { name, stops, is_roundtrip };
}