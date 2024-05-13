#include "map_renderer.h"

bool IsZero(double value)
{
    {
        return std::abs(value) < EPSILON;
    }
}

std::vector<svg::Polyline> renderer::MapRenderer::DrawRoad(const map_bus_index& bus_index, const unordered_map_stop_index& stop_index, SphereProjector& sphere_projector)
{
    std::vector<svg::Polyline> polylines;
    size_t color_num = 0;
    for (const auto& [_, bus_ptr] : bus_index) {
        std::vector<std::string> stops = bus_ptr->stops;
        if (!bus_ptr->is_roundtrip) {
            stops.insert(stops.end(), std::next(bus_ptr->stops.rbegin()), bus_ptr->stops.rend());
        }
        svg::Polyline polyline;
        for (std::string& stop : stops) {
            auto& coords = stop_index.at(stop)->coord;
            polyline.AddPoint(sphere_projector({ coords.lat, coords.lng }));
        }
        polyline.SetStrokeColor(render_settings_.color_palette[color_num]);
        polyline.SetFillColor("none");
        polyline.SetStrokeWidth(render_settings_.line_width);
        polyline.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
        polyline.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        if (color_num < (render_settings_.color_palette.size() - 1)) { ++color_num; }
        else { color_num = 0; }

        polylines.push_back(polyline);
    }
    return polylines;
}

std::vector<svg::Text> renderer::MapRenderer::DrawBusName(const map_bus_index& bus_index, const unordered_map_stop_index& stop_index, SphereProjector& sphere_projector)
{
    std::vector<svg::Text> bus_names;
    size_t color_num = 0;
    for (const auto& [_, bus_ptr] : bus_index) {
        if (bus_ptr->stops.empty()) continue;
        svg::Text text;
        svg::Text underlayer;
        text.SetPosition(sphere_projector(stop_index.at(bus_ptr->stops[0])->coord));
        text.SetOffset(render_settings_.bus_label_offset);
        text.SetFontSize(render_settings_.bus_label_font_size);
        text.SetFontFamily("Verdana");
        text.SetFontWeight("bold");
        text.SetData(bus_ptr->bus_name);
        text.SetFillColor(render_settings_.color_palette[color_num]);

        if (color_num < (render_settings_.color_palette.size() - 1)) { ++color_num; }
        else { color_num = 0; }

        underlayer.SetPosition(sphere_projector(stop_index.at(bus_ptr->stops[0])->coord));
        underlayer.SetOffset(render_settings_.bus_label_offset);
        underlayer.SetFontSize(render_settings_.bus_label_font_size);
        underlayer.SetFontFamily("Verdana");
        underlayer.SetFontWeight("bold");
        underlayer.SetData(bus_ptr->bus_name);
        underlayer.SetFillColor(render_settings_.underlayer_color);
        underlayer.SetStrokeColor(render_settings_.underlayer_color);
        underlayer.SetStrokeWidth(render_settings_.underlayer_width);
        underlayer.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
        underlayer.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

        bus_names.emplace_back(underlayer);
        bus_names.emplace_back(text);

        if (!bus_ptr->is_roundtrip && bus_ptr->stops[0] != bus_ptr->stops[bus_ptr->stops.size() - 1]) {
            svg::Text text_2{ text };
            svg::Text underlayer_2{ underlayer };
            text_2.SetPosition(sphere_projector(stop_index.at(bus_ptr->stops[bus_ptr->stops.size() - 1])->coord));
            underlayer_2.SetPosition(sphere_projector(stop_index.at(bus_ptr->stops[bus_ptr->stops.size() - 1])->coord));

            bus_names.emplace_back(underlayer_2);
            bus_names.emplace_back(text_2);
        }
    }
    return bus_names;
}

std::vector<svg::Circle> renderer::MapRenderer::DrawStopSymbol(const map_stop_index& stop_index, SphereProjector& sphere_projector) const
{
    std::vector<svg::Circle> stop_symbol;
    for (const auto& [stop_name, stop_ptr] : stop_index) {
        svg::Circle symbol;
        symbol.SetCenter(sphere_projector(stop_ptr->coord));
        symbol.SetRadius(render_settings_.stop_radius);
        symbol.SetFillColor("white");

        stop_symbol.emplace_back(symbol);
    }

    return stop_symbol;
}

std::vector<svg::Text> renderer::MapRenderer::DrawStopName(const map_stop_index& stop_index, SphereProjector& sphere_projector) const
{
    std::vector<svg::Text> stop_names;
    svg::Text text;
    svg::Text underlayer;
    for (const auto& [stop_name, stop_ptr] : stop_index) {
        text.SetPosition(sphere_projector(stop_ptr->coord));
        text.SetOffset(render_settings_.stop_label_offset);
        text.SetFontSize(render_settings_.stop_label_font_size);
        text.SetFontFamily("Verdana");
        text.SetData(stop_ptr->stop_name);
        text.SetFillColor("black");

        underlayer.SetPosition(sphere_projector(stop_ptr->coord));
        underlayer.SetOffset(render_settings_.stop_label_offset);
        underlayer.SetFontSize(render_settings_.stop_label_font_size);
        underlayer.SetFontFamily("Verdana");
        underlayer.SetData(stop_ptr->stop_name);
        underlayer.SetFillColor(render_settings_.underlayer_color);
        underlayer.SetStrokeColor(render_settings_.underlayer_color);
        underlayer.SetStrokeWidth(render_settings_.underlayer_width);
        underlayer.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
        underlayer.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

        stop_names.push_back(underlayer);
        stop_names.push_back(text);
    }
    return stop_names;
}

svg::Document renderer::MapRenderer::GetRoadMap(std::map<std::string_view, const catalogue::Bus*> bus_index, std::unordered_map<std::string_view, const catalogue::Stop*> stop_index)
{
    using namespace catalogue;
    svg::Document road_map;
    std::vector<geo::Coordinates> stops_coords;
    std::map<std::string_view, const catalogue::Stop*> stops_to_bus;

    for (const auto& [_, bus_ptr] : bus_index) {
        std::vector<std::string> stops = bus_ptr->stops;
        for (std::string& stop : stops) {
            auto& coords = stop_index.at(stop)->coord;
            stops_coords.push_back(coords);
            stops_to_bus[stop_index.at(stop)->stop_name] = stop_index.at(stop);
        }
    }

    SphereProjector sphere_projector(stops_coords.begin(), stops_coords.end(), render_settings_.width, render_settings_.height, render_settings_.padding);

    for (const auto& polyline : DrawRoad(bus_index, stop_index, sphere_projector)) {
        road_map.Add(polyline);
    }
    for (const auto& text : DrawBusName(bus_index, stop_index, sphere_projector)) {
        road_map.Add(text);
    }
    for (const auto& circle : DrawStopSymbol(stops_to_bus, sphere_projector)) {
        road_map.Add(circle);
    }
    for (const auto& text : DrawStopName(stops_to_bus, sphere_projector)) {
        road_map.Add(text);
    }
    return road_map;
}
