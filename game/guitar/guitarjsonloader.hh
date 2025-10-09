#pragma once

#include "stringprovider.hh"

#include <memory>
#include <string>

class GuitarJsonLoader {
public:
	static std::shared_ptr<StringProvider> load(std::string const& path);
};
