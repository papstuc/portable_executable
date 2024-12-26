#include "export_directory.hpp"

portable_executable::exports_iterator_t::exports_iterator_t(const std::uint8_t* module, const std::uint32_t* names, const std::uint32_t* functions, const std::uint16_t* ordinals, std::uint32_t index) :
	module(module), names(names), functions(functions), ordinals(ordinals), index(index)
{

}

portable_executable::exports_iterator_t::value_type portable_executable::exports_iterator_t::operator*() const
{
	std::uint32_t name_offset = this->names[this->index];

	if (!name_offset)
	{
		return { };
	}

	std::uint16_t ordinal_offset = this->ordinals[this->index];

	if (!ordinal_offset)
	{
		return { };
	}

	std::uint32_t functions_offset = this->functions[ordinal_offset];

	if (!functions_offset)
	{
		return { };
	}

	return
	{
		reinterpret_cast<const char*>(this->module + name_offset),
		const_cast<std::uint8_t*>(this->module + functions_offset)
	};
}

portable_executable::exports_iterator_t& portable_executable::exports_iterator_t::operator++()
{
	++this->index;

	return *this;
}

bool portable_executable::exports_iterator_t::operator==(const exports_iterator_t& other)
{
	return this->index == other.index;
}

bool portable_executable::exports_iterator_t::operator!=(const exports_iterator_t& other)
{
	return this->index != other.index;
}
