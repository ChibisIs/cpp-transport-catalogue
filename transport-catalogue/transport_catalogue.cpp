#include "transport_catalogue.h"

namespace catalogue {
	void TransportCatalogue::AddStop(const std::string& name, geo::Coordinates& coordinates) {
		stops_.push_back({ name, std::move(coordinates), {} , {} });
		auto& name_str = stops_.back().stop_name;
		stop_index_.insert({ name_str , &stops_.back() });
	}

	std::optional<Information> TransportCatalogue::GetBusStat(const std::string_view& bus_name) const
	{
		Information information{};

		auto bus_info = BusInfo(bus_name);
		if (!bus_info) throw std::invalid_argument("bus not found");

		information.name = bus_info->bus_name;
		auto& stops = bus_info->stops;

		if (!bus_info->is_roundtrip) information.stops = (stops.size() * 2) - 1;
		else information.stops = stops.size();
		double geo = 0.0;
		double fact = 0.0;
		if (bus_info->is_roundtrip) {
			for (size_t n = 0; n < stops.size() - 1; n++) {
				fact += GetDistance(stops[n], stops[n + 1]);
				geo += geo::ComputeDistance(StopInfo(stops[n])->coord, StopInfo(stops[n + 1])->coord);
			}
		}
		else {
			for (size_t n = 0; n < stops.size() - 1; n++) {
				fact += GetDistance(stops[n], stops[n + 1]) + GetDistance(stops[n + 1], stops[n]);
				geo += geo::ComputeDistance(StopInfo(stops[n])->coord, StopInfo(stops[n + 1])->coord) * 2;

			}
		}
		information.length = fact;
		information.curvature = fact / geo;
		std::unordered_set<std::string_view> unique_stops;
		for (const auto& s : stops) {
			unique_stops.insert(s);
		}
		information.unique_stops = unique_stops.size();

		return information;
	}

	void TransportCatalogue::AddBus(const std::string& name,const std::vector<std::string>& stops, bool is_roundtrip) {
		buses_.push_back({ name, stops, is_roundtrip });
		auto& name_str = buses_.back().bus_name;
		bus_index_.insert({ name_str , &buses_.back() });
		for (auto& stop : BusInfo(name)->stops) {
			for (auto& [name_, coord_, _, buses_by_stop_] : stops_) {
				if (stop == name_) {
					buses_by_stop_.insert(name);
				}
			}
		}
	}

	const Bus* TransportCatalogue::BusInfo(const std::string_view& name) const
	{
		if (!bus_index_.count(name)) {
			return nullptr;
		}
		return bus_index_.at(name);
	}

	const Stop* TransportCatalogue::StopInfo(const std::string_view& name) const
	{
		if (!stop_index_.count(name)) {
			return nullptr;
		}
		return stop_index_.at(name);
	}

	double TransportCatalogue::GetDistance(std::string_view from, std::string_view to) const
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

	const std::map<std::string_view, const Bus*> TransportCatalogue::GetBusIndex() const
	{
		std::map<std::string_view, const Bus*> result;
		for (auto& bus : bus_index_) {
			result.emplace(bus);
		}
		return result;
	}

	const std::unordered_map<std::string_view, const Stop*> TransportCatalogue::GetStopIndex() const
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
