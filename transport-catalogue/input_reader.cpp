#include "input_reader.h"

#include <algorithm>
#include <cassert>
#include <iterator>

namespace catalogue {

    namespace input {
        /**
         * Парсит строку вида "10.123,  -30.1837" и возвращает пару координат (широта, долгота)
         */
        geo::Coordinates ParseCoordinates(std::string_view str) {
            static const double nan = std::nan("");

            auto not_space = str.find_first_not_of(' ');
            auto comma = str.find(',');

            if (comma == str.npos) {
                return { nan, nan };
            }

            auto not_space2 = str.find_first_not_of(' ', comma + 1);
            auto comma2 = str.find_first_of(',', not_space2);

            double lat = std::stod(std::string(str.substr(not_space, comma - not_space)));
            double lng;
            if (comma2 == str.npos) {
                lng = std::stod(std::string(str.substr(not_space2)));
            }
            else {
                lng = std::stod(std::string(str.substr(not_space2, comma2 - not_space2)));
            }
            return { lat, lng };
        }

        /**
         * Удаляет пробелы в начале и конце строки
         */
        std::string_view Trim(std::string_view string) {
            const auto start = string.find_first_not_of(' ');
            if (start == string.npos) {
                return {};
            }
            return string.substr(start, string.find_last_not_of(' ') + 1 - start);
        }

        /**
         * Разбивает строку string на n строк, с помощью указанного символа-разделителя delim
         */
        std::vector<std::string_view> Split(std::string_view string, char delim) {
            std::vector<std::string_view> result;

            size_t pos = 0;
            while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
                auto delim_pos = string.find(delim, pos);
                if (delim_pos == string.npos) {
                    delim_pos = string.size();
                }
                if (auto substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty()) {
                    result.push_back(substr);
                }
                pos = delim_pos + 1;
            }

            return result;
        }

        /**
         * Парсит маршрут.
         * Для кольцевого маршрута (A>B>C>A) возвращает массив названий остановок [A,B,C,A]
         * Для некольцевого маршрута (A-B-C-D) возвращает массив названий остановок [A,B,C,D,C,B,A]
         */
        std::vector<std::string_view> ParseRoute(std::string_view route) {
            if (route.find('>') != route.npos) {
                return Split(route, '>');
            }

            auto stops = Split(route, '-');
            std::vector<std::string_view> results(stops.begin(), stops.end());
            results.insert(results.end(), std::next(stops.rbegin()), stops.rend());

            return results;
        }

        static CommandDescription ParseCommandDescription(std::string_view line) {
            // Exsample (Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka)
            auto colon_pos = line.find(':');
            if (colon_pos == line.npos) {
                return {};
            }

            auto space_pos = line.find(' ');
            if (space_pos >= colon_pos) {
                return {};
            }

            auto not_space = line.find_first_not_of(' ', space_pos);
            if (not_space >= colon_pos) {
                return {};
            }

            return { std::string(line.substr(0, space_pos)), //(Bus)
                    std::string(line.substr(not_space, colon_pos - not_space)), //(750)
                    std::string(line.substr(colon_pos + 1)) }; //(Tolstopaltsevo - Marushkino - Rasskazovka)
        }

        void InputReader::ParseLine(std::string_view line) {
            auto command_description = ParseCommandDescription(line);
            if (command_description) {
                commands_.push_back(std::move(command_description));
            }
        }

        std::vector<std::pair<int, std::string>> InputReader::ParseDistance(std::string_view str) const
        {
            std::vector<std::pair<int, std::string>> dist;
            str = str.substr(str.find(",") + 1); // 37.20829, 3900m to Marushkino, 750m to Universam
            auto str_distance = str.substr(str.find_first_of(",") + 1); // 3900m to Marushkino, 750m to Universam
            auto comma = str_distance.find(" to ") + 1;

            while (comma != 0) {
                auto meters = std::stoi(std::string(str_distance.substr(0, str_distance.find_first_of("m"))));
                auto stop = std::string(str_distance.substr(str_distance.find_first_of("m") + 5,
                    str_distance.find(",") - str_distance.find_first_of("m") - 5));
                dist.push_back({ meters, stop });
                comma = str_distance.find(",") + 1;
                str_distance = str_distance.substr(str_distance.find_first_of(",") + 1);
            }
            return dist;
        }

        void InputReader::ApplyCommands([[maybe_unused]] catalogue::TransportCatalogue& catalogue) const {

            for (auto& [c, i, d] : commands_) {
                if (c == "Bus") {
                    auto stops_to_bus = ParseRoute(d);
                    std::vector<std::string> new_stops;
                    for (auto& s : stops_to_bus) {
                        new_stops.push_back(std::string(s));
                    }
                    catalogue.AddBus(i, new_stops);
                }
                else if (c == "Stop") {
                    auto cootdinates = ParseCoordinates(d);
                    catalogue.AddStop(i, cootdinates);
                }
            }
            for (auto& [c, i, d] : commands_) {
                if (c == "Stop") {
                    auto dist = ParseDistance(d);
                    for (auto& [m, stop] : dist) {
                        catalogue.AddDistance(i, stop, m);
                    }
                }
            }
        }
    } //namespace input 

} //namespace catalogue 