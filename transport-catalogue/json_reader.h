#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "request_handler.h"


#include <iostream>
using namespace catalogue;

class JsonReader {
public:
    JsonReader(std::istream& input, TransportCatalogue& catalogue)
        : input_(json::Load(input))
    {
        FillCatalogue(catalogue);
    }

    void ProcessRequest(const json::Node& requests_, RequestHandler& handler, TransportCatalogue& catalogue) const;

    const json::Node& GetBaseRequests() const;
    const json::Node& GetRenderSettings() const;
    const json::Node& GetStatRequests() const;

    void FillCatalogue(TransportCatalogue& catalogue);
    RenderSettings FillRenderSettings(const json::Dict& request_map) const;


private:
    json::Document input_;

    std::tuple<std::string, geo::Coordinates, std::map<std::string_view, int>> FillStop(const json::Dict& request_map) const;
    std::tuple <std::string, std::vector<std::string>, bool> FillBus(const json::Dict& request_map) const;

    void FillStopDistances(TransportCatalogue& catalogue) const;
};