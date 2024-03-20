#include "transport_catalogue.h"

namespace catalogue {

	void TransportCatalogue::AddStop(std::string& name, geo::Coordinates& coordinates) {
		stops_.push_back({ name, std::move(coordinates) });
		auto& name_str = stops_.back().stop_name;
		stop_index_.insert({ name_str , &stops_.back() });
	}

	void TransportCatalogue::AddBus(std::string& name, std::vector<std::string>& stops) {
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

	const Information TransportCatalogue::Info(std::string_view& name) {
		Information information{};
		if (!BusInfo(name)) throw std::invalid_argument("bus not found");

		information.name = BusInfo(name)->bus_name;
		auto stops = BusInfo(name)->stops;
		information.stops = stops.size();

		std::unordered_set<std::string_view> unique_stops;
		for (const auto& s : stops) {
			unique_stops.insert(s);
		}
		information.unique_stops = unique_stops.size();
		information.length = AllComputeDistance(stops);

		return information;
	}

	double TransportCatalogue::AllComputeDistance(const std::vector<std::string>& stops) {
		double length = 0.0;
		for (int n = 0; n < stops.size() - 1; n++) {
			length += ComputeDistance(StopInfo(stops[n])->coord, StopInfo(stops[n + 1])->coord);
		}
		return length;
	};

	std::set<std::string> TransportCatalogue::BusesToStop(std::string name) {
		std::set<std::string> buses;
		for (auto& s : buses_) {
			for (auto& t : s.stops) {
				if (t == name) {
					buses.insert(s.bus_name);
				}
			}
		}
		return buses;
	}
} // namespace catalogue