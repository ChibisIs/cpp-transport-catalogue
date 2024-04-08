#include "stat_reader.h"

using namespace std::literals;

namespace catalogue {
    namespace output {

        void StatReader::ParseAndPrintStat(TransportCatalogue& tansport_catalogue, std::string_view request,
            std::ostream& output) {
            std::string_view command = request.substr(0, request.find_first_of(" "));
            if (command == "Bus")
            {
                std::string_view name = request.substr(request.find_first_of(" ") + 1);
                const Bus* bus = tansport_catalogue.BusInfo(name);
                if (bus == nullptr) {
                    output << "Bus "s << name << ": not found"s << std::endl;
                }
                else {
                    auto inf = tansport_catalogue.Info(name);
                    output << "Bus "s << name << ": "s <<
                        inf.stops << " stops on route, " <<
                        inf.unique_stops << " unique stops, " <<
                        inf.length << " route length, " << std::setprecision(6) <<
                        inf.curvature << " curvature" << std::endl;
                }
            }
            else if (command == "Stop")
            {
                std::string_view name = request.substr(request.find_first_of(" ") + 1);
                const Stop* stop = tansport_catalogue.StopInfo(name);
                if (stop == nullptr) {
                    output << "Stop "s << name << ": not found"s << std::endl;
                }
                else if (tansport_catalogue.BusesToStop(std::string(name)).empty()) {
                    output << "Stop "s << name << ": no buses"s << std::endl;
                }
                else {
                    output << "Stop "s << name << ": buses "s;
                    for (auto& s : tansport_catalogue.BusesToStop(std::string(name))) {
                        output << s << " ";
                    }
                    output << std::endl;
                }
            }
        }

    } //    namespace output

} // namespace catalogue