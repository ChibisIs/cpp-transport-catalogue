#include "transport_router.h"

namespace catalogue {

    const graph::DirectedWeightedGraph<double>& Router::BuildGraph(const TransportCatalogue& catalogue)
    {
        const auto& all_stops = catalogue.GetSortedAllStops();
        const auto& all_buses = catalogue.GetBusIndex();

        graph::DirectedWeightedGraph<double> stops_graph(all_stops.size() * 2);
        std::map<std::string, graph::VertexId> stops_id;
        graph::VertexId vertex_id = 0;

        for (const auto& [_, stop_info] : all_stops) {
            stops_id[stop_info->stop_name] = vertex_id;
            stops_graph.AddEdge({ stop_info->stop_name, 0,
                                  vertex_id, ++vertex_id,
                                  static_cast<double>(bus_wait_time_) });
            ++vertex_id;
        }
        stops_id_ = std::move(stops_id);

        //
        double bus_velocity_conv = 100.0 / 6.0;
        for (const auto& info : all_buses) {
            const auto& bus = info.second;
            const auto& stops_list = bus->stops;
            size_t stops_count = stops_list.size();

            for (size_t i = 0; i < stops_count; ++i) 
            {
                for (size_t j = i + 1; j < stops_count; ++j) {
                    const Stop* stop_from = catalogue.StopInfo(stops_list[i]);
                    const Stop* stop_to = catalogue.StopInfo(stops_list[j]);
                    int dist_sum = 0;
                    int dist_sum_inverse = 0;

                    for (size_t k = i + 1; k <= j; ++k) {
                        dist_sum += catalogue.GetDistance(stops_list[k - 1], stops_list[k]);
                        dist_sum_inverse += catalogue.GetDistance(stops_list[k], stops_list[k - 1]);
                    }

                    stops_graph.AddEdge({ bus->bus_name, j - i,
                                                       stops_id_.at(stop_from->stop_name) + 1,
                                                       stops_id_.at(stop_to->stop_name),
                                                       static_cast<double>(dist_sum) / (bus_velocity_ * bus_velocity_conv) });

                    if (!bus->is_roundtrip) {
                        stops_graph.AddEdge({ bus->bus_name, j - i,
                                                           stops_id_.at(stop_to->stop_name) + 1,
                                                           stops_id_.at(stop_from->stop_name),
                                                           static_cast<double>(dist_sum_inverse) / (bus_velocity_ * bus_velocity_conv) });
                    }
                }
            }
        }

        graph_ = std::move(stops_graph);
        router_ = new graph::Router<double>(graph_);

        return graph_;
    }

    const std::optional<graph::Router<double>::RouteInfo> Router::FindRoute(const std::string_view stop_from, const std::string_view stop_to) const
    {
        return router_->BuildRoute(stops_id_.at(std::string(stop_from)), stops_id_.at(std::string(stop_to)));
    }

    const graph::Edge<double>& Router::GetGraphEdge(graph::EdgeId edge_id) const
    {
        return graph_.GetEdge(edge_id);
    }
}