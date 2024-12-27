#include "image.hpp"

portable_executable::dos_header_t* portable_executable::image_t::dos_header()
{
	return &this->m_dos_header;
}

const portable_executable::dos_header_t* portable_executable::image_t::dos_header() const
{
	return &this->m_dos_header;
}

portable_executable::nt_headers_t* portable_executable::image_t::nt_headers()
{
	return this->dos_header()->nt_headers();
}

const portable_executable::nt_headers_t* portable_executable::image_t::nt_headers() const
{
	return this->dos_header()->nt_headers();
}

portable_executable::section_header_t* portable_executable::image_t::find_section(std::string_view name)
{
	for (auto& section : this->sections())
	{
		if (section.to_str().find(name) != std::string::npos)
		{
			return &section;
		}
	}

	return nullptr;
}

std::uint8_t* portable_executable::image_t::find_export(std::string_view name) const
{
	for (const auto& [export_name, export_address] : this->exports())
	{
		if (export_name.find(name) != std::string::npos)
		{
			return export_address;
		}
	}

	return nullptr;
}
