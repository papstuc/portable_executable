#include "file.hpp"
#include "image.hpp"

#include <fstream>

portable_executable::file_t::file_t(const std::string_view file_path) : m_file_path(file_path)
{
}

portable_executable::file_t::file_t(const std::wstring_view file_path) : m_file_path(file_path)
{
}

portable_executable::file_t::file_t(std::filesystem::path file_path) : m_file_path(std::move(file_path))
{
}

bool portable_executable::file_t::load()
{
	std::ifstream file_stream(this->m_file_path, std::ios::binary);

	if (!file_stream.is_open())
	{
		return false;
	}

	file_stream.seekg(0, std::ios::end);

	const std::streampos file_size = file_stream.tellg();

	file_stream.seekg(0, std::ios::beg);

	std::vector<std::uint8_t> raw_buffer(file_size);
	file_stream.read(reinterpret_cast<char*>(raw_buffer.data()), static_cast<std::streamsize>(raw_buffer.size()));

	const auto raw_image = reinterpret_cast<const image_t*>(raw_buffer.data());

	if (!raw_image->dos_header()->valid() || !raw_image->nt_headers()->valid())
	{
		return false;
	}

	this->m_buffer.resize(raw_image->nt_headers()->optional_header.size_of_image, 0);

	std::memcpy(this->m_buffer.data(), raw_buffer.data(), 0x1000);

	for (const auto& section : raw_image->sections())
	{
		std::memcpy(this->m_buffer.data() + section.virtual_address, raw_buffer.data() + section.pointer_to_raw_data, section.size_of_raw_data);
	}

	return true;
}

portable_executable::image_t* portable_executable::file_t::image()
{
	return reinterpret_cast<image_t*>(this->m_buffer.data());
}

const portable_executable::image_t* portable_executable::file_t::image() const
{
	return reinterpret_cast<const image_t*>(this->m_buffer.data());
}
