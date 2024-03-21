#include <iostream>
#include <string>

#include "input_reader.h"
#include "stat_reader.h"

using namespace catalogue;

int main() {
    catalogue::TransportCatalogue catalogue;

    {
        input::InputReader reader(std::cin, catalogue);
    }
    {
        output::StatReader reader(std::cin, std::cout, catalogue);
    }
}
