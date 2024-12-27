#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <filesystem>

namespace portable_executable
{
	class image_t;

	class file_t
	{
	private:
		std::filesystem::path m_file_path;

		std::vector<std::uint8_t> m_buffer;

	public:
		file_t(std::string_view file_path);

		file_t(std::wstring_view file_path);

		bool load();

		image_t* image();

		const image_t* image() const;
	};
}