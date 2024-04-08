#pragma once
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "geo.h"
#include "transport_catalogue.h"

namespace catalogue {

    namespace input {

        struct CommandDescription {
            // Определяет, задана ли команда (поле command непустое)
            explicit operator bool() const {
                return !command.empty();
            }

            bool operator!() const {
                return !operator bool();
            }

            std::string command;      // Название команды
            std::string id;           // id маршрута или остановки
            std::string description;  // Параметры команды
        };

        class InputReader {
        public:
            InputReader(std::istream& in, TransportCatalogue& catalogue) {

                int base_request_count;
                in >> base_request_count >> std::ws;

                for (int i = 0; i < base_request_count; ++i) {
                    std::string line;
                    getline(in, line);
                    ParseLine(line);
                }
                ApplyCommands(catalogue);
            }
            /**
             * Парсит строку в структуру CommandDescription и сохраняет результат в commands_
             */
            void ParseLine(std::string_view line);

            std::vector<std::pair<int, std::string>> ParseDistance(std::string_view str) const;

            /**
             * Наполняет данными транспортный справочник, используя команды из commands_
             */
            void ApplyCommands(TransportCatalogue& catalogue) const;

        private:
            std::vector<CommandDescription> commands_;
        };
    } //namespace input

} //namespace catalogue 