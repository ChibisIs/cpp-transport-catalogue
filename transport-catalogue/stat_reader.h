#pragma once

#include "transport_catalogue.h"

#include <algorithm>
#include <iomanip>
#include <iosfwd>
#include <iostream>
#include <string>
#include <string_view>

namespace catalogue {

    namespace output {

        class StatReader {
        public:
            StatReader(std::istream& in, std::ostream& out, TransportCatalogue& catalogue) {
                int stat_request_count;
                in >> stat_request_count >> std::ws;

                for (int i = 0; i < stat_request_count; ++i) {
                    std::string line;
                    getline(in, line);
                    ParseAndPrintStat(catalogue, line, out);
                }
            }

            void ParseAndPrintStat(TransportCatalogue& tansport_catalogue, std::string_view request,
                std::ostream& output);
        };

    }//    namespace output 

} // namespace catalogue 
