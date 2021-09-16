#include "util.hh"

// Only conversion types used in Performous are provided
template <> int sconv(std::string const& s) { return std::stoi(s); }
template <> unsigned int sconv(std::string const& s) { return std::stoul(s); }
template <> double sconv(std::string const& s) { return std::stod(s); }
template <> std::string sconv(std::string const& s) { return s; }


std::string toLower(std::string const& s) {
    auto result = s;
    
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c){ return std::tolower(c); });
    
    return result;
}

std::string toUpper(std::string const& s) {
    auto result = s;
    
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c){ return std::toupper(c); });
    
    return result;
}

bool containsNoCase(std::string const& word, std::string const& part) {
    return toLower(word).find(toLower(part)) != std::string::npos;
}

