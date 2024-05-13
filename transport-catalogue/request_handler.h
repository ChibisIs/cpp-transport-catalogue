#pragma once
#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"

#include <algorithm>
#include <sstream>
#include <optional>

using namespace catalogue;

class RequestHandler {
public:
    // MapRenderer понадобится в следующей части итогового проекта
    RequestHandler(TransportCatalogue& db, renderer::MapRenderer& renderer)
        : db_(db),
        renderer_(renderer)
    {

    }
    // Возвращает информацию о маршруте (запрос Bus)
    std::optional<Information> GetBusStat(const std::string_view& bus_name) const;

    // Возвращает маршруты, проходящие через
    const std::set<std::string> GetBusesByStop(const std::string_view& stop_name) const;

    // Этот метод будет нужен в следующей части итогового проекта
    svg::Document RenderMap() const;

private:
    TransportCatalogue& db_;
    renderer::MapRenderer& renderer_;
};
