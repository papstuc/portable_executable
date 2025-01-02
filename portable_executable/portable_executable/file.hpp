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
		std::filesystem::path m_file_path;

		std::vector<std::uint8_t> m_buffer;

	public:
		explicit file_t(std::string_view file_path);

		explicit file_t(std::wstring_view file_path);

		explicit file_t(std::filesystem::path file_path);

		bool load();

		image_t* image();

		[[nodiscard]] const image_t* image() const;
	};
}