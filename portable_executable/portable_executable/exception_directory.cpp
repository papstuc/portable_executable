#include "exception_directory.hpp"

portable_executable::unwind_code_iterator_t portable_executable::unwind_info_t::begin() const
{
	return unwind_code_iterator_t{ codes };
}

portable_executable::unwind_code_iterator_t portable_executable::unwind_info_t::end() const
{
	return unwind_code_iterator_t{ codes + unwind_code_count };
}

portable_executable::unwind_code_iterator_t::value_type portable_executable::unwind_code_iterator_t::operator*() const
{
	return *m_current_code;
}

portable_executable::unwind_code_iterator_t& portable_executable::unwind_code_iterator_t::operator++()
{
	++m_current_code;

	return *this;
}

bool portable_executable::unwind_code_iterator_t::operator==(const unwind_code_iterator_t& other) const
{
	return m_current_code == other.m_current_code;
}

bool portable_executable::unwind_code_iterator_t::operator!=(const unwind_code_iterator_t& other) const
{
	return m_current_code != other.m_current_code;
}

portable_executable::runtime_functions_iterator_t::value_type portable_executable::runtime_functions_iterator_t::operator*() const
{
	const auto function_begin = const_cast<std::uint8_t*>(m_module + m_current_function->begin_address);
	const auto function_end = const_cast<std::uint8_t*>(m_module + m_current_function->end_address);
	const auto unwind_info = reinterpret_cast<unwind_info_t*>(const_cast<std::uint8_t*>(m_module + m_current_function->unwind_info_rva));

	return value_type{ function_begin, function_end, unwind_info };
}

portable_executable::runtime_functions_iterator_t& portable_executable::runtime_functions_iterator_t::operator++()
{
	++m_current_function;

	return *this;
}

bool portable_executable::runtime_functions_iterator_t::operator==(const runtime_functions_iterator_t& other) const
{
	return m_current_function == other.m_current_function;
}

bool portable_executable::runtime_functions_iterator_t::operator!=(const runtime_functions_iterator_t& other) const
{
	return m_current_function != other.m_current_function;
}
