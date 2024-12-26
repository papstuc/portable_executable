#include "section_header.hpp"

std::string portable_executable::section_header_t::to_str() const
{
	return this->name;
}
