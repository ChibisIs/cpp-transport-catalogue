#include "transport_catalogue.h"

namespace catalogue {

	void TransportCatalogue::AddStop(const std::string& name, geo::Coordinates& coordinates) {
		stops_.push_back({ name, std::move(coordinates) });
		auto& name_str = stops_.back().stop_name;
		stop_index_.insert({ name_str , &stops_.back() });
	}

	void TransportCatalogue::AddBus(const std::string& name, std::vector<std::string>& stops) {
		buses_.push_back({ name, std::move(stops) });
		auto& name_str = buses_.back().bus_name;
		bus_index_.insert({ name_str , &buses_.back() });
	}

	const Bus* TransportCatalogue::BusInfo(const std::string_view& name) {
		if (!bus_index_.count(name)) {
			return nullptr;
		}
		return bus_index_.at(name);
	}

	const Stop* TransportCatalogue::StopInfo(const std::string_view& name) {
		if (!stop_index_.count(name)) {
			return nullptr;
		}
		return stop_index_.at(name);
	}

	const Information TransportCatalogue::GetBusStatistics(const std::string_view& name) {
		Information information{};
		auto bus_info = BusInfo(name);
		if (!bus_info) throw std::invalid_argument("bus not found");

		information.name = bus_info->bus_name;
		auto stops = bus_info->stops;
		information.stops = stops.size();

		std::unordered_set<std::string_view> unique_stops;
		for (const auto& s : stops) {
			unique_stops.insert(s);
		}
		information.unique_stops = unique_stops.size();
		information.length = AllComputeDistance(stops);
		information.curvature = GetCurvature(stops);

		return information;
	}

	double TransportCatalogue::AllComputeDistance(const std::vector<std::string>& stops) {
		double length = 0.0;

		for (int n = 0; n < stops.size() - 1; n++) {
			length += GetDistance(stops[n], stops[n + 1]);
		}

		return length;
	}

	double TransportCatalogue::GetDistance(std::string_view from, std::string_view to)
	{
		auto from_ptr = StopInfo(from);
		auto to_ptr = StopInfo(to);
		if (distance_index_.count({ from_ptr, to_ptr })) {
			return distance_index_.at({ from_ptr, to_ptr });
		}
		else if (distance_index_.count({ to_ptr, from_ptr })) {
			return distance_index_.at({ to_ptr, from_ptr });
		}
		return 0.;
	}

	double TransportCatalogue::GetCurvature(const std::vector<std::string>& stops)
	{
		double geo = 0.0;

		auto fact = AllComputeDistance(stops);
		for (int n = 0; n < stops.size() - 1; n++) {
			geo += ComputeDistance(StopInfo(stops[n])->coord, StopInfo(stops[n + 1])->coord);
		}

		return fact / geo;
	}


	std::set<std::string> TransportCatalogue::BusesToStop(const std::string& name) {
		std::set<std::string> buses;
		for (auto& [n, stops] : buses_) {
			for (auto& stop : stops) {
				if (stop == name) {
					buses.insert(n);
				}
			}
		}
		return buses;
	}
	void TransportCatalogue::AddDistance(std::string_view from, std::string_view to, double distance)
	{
		auto from_ptr = StopInfo(from);
		auto to_ptr = StopInfo(to);
		distance_index_[{from_ptr, to_ptr}] = distance;
	}
} // namespace catalogue
