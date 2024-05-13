#include "transport_catalogue.h"

namespace catalogue {
	void TransportCatalogue::AddStop(const std::string& name, geo::Coordinates& coordinates) {
		stops_.push_back({ name, std::move(coordinates), {} });
		auto& name_str = stops_.back().stop_name;
		stop_index_.insert({ name_str , &stops_.back() });
	}

	void TransportCatalogue::AddBus(const std::string& name, std::vector<std::string>& stops, bool is_roundtrip) {
		buses_.push_back({ name, std::move(stops), is_roundtrip });
		auto& name_str = buses_.back().bus_name;
		bus_index_.insert({ name_str , &buses_.back() });
		for (auto& stop : BusInfo(name)->stops) {
			for (auto& [name_, _, buses_by_stop_] : stops_) {
				if (stop == name_) {
					buses_by_stop_.insert(name);
				}
			}
		}
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

	std::map<std::string_view, const Bus*> TransportCatalogue::GetBusIndex() const
	{
		std::map<std::string_view, const Bus*> result;
		for (auto& bus : bus_index_) {
			result.emplace(bus);
		}
		return result;
	}

	std::unordered_map<std::string_view, const Stop*> TransportCatalogue::GetStopIndex() const
	{
		return stop_index_;
	}

	void TransportCatalogue::AddDistance(std::string_view from, std::string_view to, double distance)
	{
		auto from_ptr = StopInfo(from);
		auto to_ptr = StopInfo(to);
		distance_index_[{from_ptr, to_ptr}] = distance;
	}
} // namespace catalogue
