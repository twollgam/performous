#include "fingering.hh"

#include <stdexcept>
#include <string>

int Fingering::operator[](int n) const {
    switch (n) {
    case 0: return e_low;
    case 1: return a;
    case 2: return d;
    case 3: return g;
    case 4: return b;
    case 5: return e_high;
    default:
        throw std::runtime_error("Index " + std::to_string(n) + " out of range (0 - 5)");
    }
}


