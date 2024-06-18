#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "json_builder.h"
#include "transport_router.h"

#include <algorithm>
#include <sstream>
#include <optional>

class RequestHandler {
public:
    RequestHandler(catalogue::TransportCatalogue& db, renderer::MapRenderer& renderer, catalogue::Router& router)
        : db_(db),
        renderer_(renderer),
        router_(router)
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
    catalogue::Router& router_;
};
