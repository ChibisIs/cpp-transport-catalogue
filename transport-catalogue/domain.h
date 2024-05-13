#pragma once

#include "geo.h"

#include <string>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>

namespace catalogue {
	struct Stop
	{
		std::string stop_name;
		geo::Coordinates coord;
		std::map<std::string_view, int> road_distances;
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
} //catalogue