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
	
	RequestHandler handler(catalogue, renderer);
reader.ProcessRequest(reader.GetStatRequests(), handler, catalogue);

}
