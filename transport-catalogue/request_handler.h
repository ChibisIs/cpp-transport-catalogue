#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "json_builder.h"


#include <algorithm>
#include <sstream>
#include <optional>

class RequestHandler {
public:
    RequestHandler(catalogue::TransportCatalogue& db, renderer::MapRenderer& renderer)
        : db_(db),
        renderer_(renderer)
    {

    }
    void ProcessRequest(const json::Node& requests_) const;

    // Возвращает маршруты, проходящие через
    const std::set<std::string> GetBusesByStop(const std::string_view& stop_name) const;

    // Этот метод будет нужен в следующей части итогового проекта
    svg::Document RenderMap() const;

private:
    catalogue::TransportCatalogue& db_;
    renderer::MapRenderer& renderer_;
};
