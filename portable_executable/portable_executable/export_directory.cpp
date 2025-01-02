#include "export_directory.hpp"

portable_executable::exports_iterator_t::exports_iterator_t(const std::uint8_t* module, const std::uint32_t* names, const std::uint32_t* functions, const std::uint16_t* ordinals, std::uint32_t index) :
	m_module(module), m_names(names), m_functions(functions), m_ordinals(ordinals), m_index(index)
{

}

portable_executable::exports_iterator_t::value_type portable_executable::exports_iterator_t::operator*() const
{
	const std::uint32_t name_offset = this->m_names[this->m_index];

	const std::uint16_t ordinal_offset = this->m_ordinals[this->m_index];

	const std::uint32_t functions_offset = this->m_functions[ordinal_offset];

	return
	{
		reinterpret_cast<const char*>(this->m_module + name_offset),
		const_cast<std::uint8_t*>(this->m_module + functions_offset)
	};
}

portable_executable::exports_iterator_t& portable_executable::exports_iterator_t::operator++()
{
	++this->m_index;

	return *this;
}

bool portable_executable::exports_iterator_t::operator==(const exports_iterator_t& other) const
{
	return this->m_index == other.m_index;
}

bool portable_executable::exports_iterator_t::operator!=(const exports_iterator_t& other) const
{
	return this->m_index != other.m_index;
}
