#pragma once

#include <deque>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "geo.h"

namespace catalogue {

	struct Stop
	{
		std::string stop_name;
		geo::Coordinates coord;
	};

	struct Bus
	{
		std::string bus_name;
		std::vector<std::string> stops;
	};

	struct Information
	{
		std::string_view name;
		size_t stops;
		size_t unique_stops;
		double length;
	};

	class TransportCatalogue {
	public:
		//добавление маршрута в базу (Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka)
								  // (Bus 256: Biryulyovo Zapadnoye > Biryusinka > Biryulyovo Tovarnaya > Biryulyovo Zapadnoye)
		void AddBus(const std::string& name, std::vector<std::string>& stops);

		//добавление остановки в базу (Stop X: latitude, longitude)
		void AddStop(const std::string& name, geo::Coordinates& coordinates);

		//поиск маршрута по имени,
		const Bus* BusInfo(const std::string_view& name);

		//поиск остановки по имени,
		const Stop* StopInfo(const std::string_view& name);

		//получение информации о маршруте (Bus X: R stops on route, U unique stops, L route length)
										//(Bus 256: 4 stops on route, 3 unique stops, 4371.02 route length)
		const Information Info(const std::string_view& name);

		//метод для получения автобусов для остановки
		std::set<std::string> BusesToStop(const std::string name);



	private:
		std::deque<Bus> buses_;
		std::deque<Stop> stops_;
		std::unordered_map<std::string_view, const Bus*> bus_index_;
		std::unordered_map<std::string_view, const Stop*> stop_index_;

		//вспомогательная функция для подсчета общего расстояния маршрута
		double AllComputeDistance(const std::vector<std::string>& stops);
	};

} // namespace catalogue