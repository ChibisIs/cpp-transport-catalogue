#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "json_builder.h"


#include <iostream>

class JsonReader {
public:
    JsonReader(std::istream& input, catalogue::TransportCatalogue& catalogue)
        : input_(json::Load(input))
    {
        FillCatalogue(catalogue);
    }
    const json::Node& GetBaseRequests() const;
    const json::Node& GetRenderSettings() const;
    const json::Node& GetStatRequests() const;
    const json::Node& GetRoutingSettings() const;

    void FillCatalogue(catalogue::TransportCatalogue& catalogue);
    renderer::MapRenderer FillRenderSettings(const json::Dict& request_map) const;
    catalogue::RouteSettings FillRoutingSettings(const json::Node& settings) const;

    

private:
    json::Document input_;

    catalogue::Stop FillStop(const json::Dict& request_map) const;
    catalogue::Bus FillBus(const json::Dict& request_map) const;
};