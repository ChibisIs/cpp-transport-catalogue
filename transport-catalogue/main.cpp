#include <iostream>
#include <string>
#include <fstream>

#include "json_reader.h"
#include "request_handler.h"

int main() {
	catalogue::TransportCatalogue catalogue;
	JsonReader reader(std::cin, catalogue);
	const auto& sett = reader.FillRenderSettings(reader.GetRenderSettings().AsMap());
	renderer::MapRenderer renderer(sett);
	const auto& routing_settings = reader.FillRoutingSettings(reader.GetRoutingSettings());
	catalogue::Router router = { routing_settings, catalogue };

	RequestHandler handler(catalogue, renderer, router);
	handler.ProcessRequest(reader.GetStatRequests());
}