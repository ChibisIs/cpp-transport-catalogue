#pragma once

#include "router.h"
#include "transport_catalogue.h"

#include <memory>

namespace catalogue {

	struct RouteSettings {
		int bus_wait_time_ = 0;
		double bus_velocity_ = 0.0;
	};

	class Router {
	public:
		Router() = default;

		Router(const RouteSettings& route_settings, const TransportCatalogue& catalogue) {
			route_settings_ = route_settings;
			BuildGraph(catalogue);
		}

		
		const std::optional<graph::Router<double>::RouteInfo> FindRoute(const std::string_view stop_from, const std::string_view stop_to) const;
		const graph::Edge<double>& GetGraphEdge(graph::EdgeId edge_id) const;

	private:
		RouteSettings route_settings_;

		const graph::DirectedWeightedGraph<double>& BuildGraph(const TransportCatalogue& catalogue);

		graph::DirectedWeightedGraph<double> graph_;
		std::map<std::string, graph::VertexId> stops_id_;
		graph::Router<double>* router_ = nullptr;
	};

} // catalogue
