#pragma once

#include <deque>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <map>

#include "geo.h"

namespace catalogue {

	struct Stop
	{
		std::string stop_name;
		geo::Coordinates coord;
		std::set<std::string> buses_by_stop;
	};

	struct Bus
	{
		std::string bus_name;
		std::vector<std::string> stops;
		bool is_roundtrip;
	};

	struct Information
	{
		std::string_view name;
		size_t stops;
		size_t unique_stops;
		double length;
		double curvature;
	};

	class TransportCatalogue {
	public:
		struct Hasher {
			size_t operator()(const std::pair<const Stop*, const Stop*>& points) const {
				size_t hash_first = std::hash<const void*>{}(points.first);
				size_t hash_second = std::hash<const void*>{}(points.second);
				return hash_first + hash_second * 37;
			}
		};
		//добавление маршрута в базу (Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka)
								  // (Bus 256: Biryulyovo Zapadnoye > Biryusinka > Biryulyovo Tovarnaya > Biryulyovo Zapadnoye)
		void AddBus(const std::string& name, std::vector<std::string>& stops, bool is_roundtrip);

		//добавление остановки в базу (Stop X: latitude, longitude)
		void AddStop(const std::string& name, geo::Coordinates& coordinates);

		//поиск маршрута по имени,
		const Bus* BusInfo(const std::string_view& name);

		//поиск остановки по имени,
		const Stop* StopInfo(const std::string_view& name);

		void AddDistance(std::string_view from, std::string_view to, double distance);

		double GetDistance(std::string_view from, std::string_view to);

		std::map<std::string_view, const Bus*> GetBusIndex() const;

		std::unordered_map<std::string_view, const Stop*> GetStopIndex() const;

	private:
		std::deque<Bus> buses_;
		std::deque<Stop> stops_;
		std::unordered_map<std::string_view, const Bus*> bus_index_;
		std::unordered_map<std::string_view, const Stop*> stop_index_;
		std::unordered_map<std::pair<const Stop*, const Stop*>, double, Hasher> distance_index_;
	};

} // namespace catalogue