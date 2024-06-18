#pragma once

#include "router.h"
#include "transport_catalogue.h"

#include <memory>

namespace catalogue {

	class Router {
	public:
		Router() = default;

		Router(const int bus_wait_time, const double bus_velocity)
			: bus_wait_time_(bus_wait_time)
			, bus_velocity_(bus_velocity)

		{}

		Router(const Router& settings, const TransportCatalogue& catalogue) {
			bus_wait_time_ = settings.bus_wait_time_;
			bus_velocity_ = settings.bus_velocity_;
			BuildGraph(catalogue);
		}

		
		const std::optional<graph::Router<double>::RouteInfo> FindRoute(const std::string_view stop_from, const std::string_view stop_to) const;
		const graph::Edge<double>& GetGraphEdge(graph::EdgeId edge_id) const;

	private:

		int bus_wait_time_ = 0;
		double bus_velocity_ = 0.0;

		const graph::DirectedWeightedGraph<double>& BuildGraph(const TransportCatalogue& catalogue);

		graph::DirectedWeightedGraph<double> graph_;
		std::map<std::string, graph::VertexId> stops_id_;
		graph::Router<double>* router_ = nullptr;
	};

} // catalogue
