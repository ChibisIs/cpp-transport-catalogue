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

        void ParseAndPrintStat(TransportCatalogue& tansport_catalogue, std::string_view request,
            std::ostream& output);
    }//    namespace output 

} // namespace catalogue 
