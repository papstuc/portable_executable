#pragma once

#include "dos_header.hpp"
#include "nt_headers.hpp"

#include "section_header.hpp"
#include "export_directory.hpp"
#include "imports_directory.hpp"
#include "relocations_directory.hpp"

namespace portable_executable
{
	class image_t
	{
		dos_header_t m_dos_header = { };

	public:
		template<typename T = std::uintptr_t>
		[[nodiscard]] T as() const
		{
			return reinterpret_cast<T>(this);
		}

		dos_header_t* dos_header();

		[[nodiscard]] const dos_header_t* dos_header() const;
		
		nt_headers_t* nt_headers();

		[[nodiscard]] const nt_headers_t* nt_headers() const;

		section_iterator_t<section_header_t> sections()
		{
			return { this->nt_headers()->section_headers(), this->nt_headers()->num_sections() };
		}

		[[nodiscard]] section_iterator_t<const section_header_t> sections() const
		{
			return { this->nt_headers()->section_headers(), this->nt_headers()->num_sections() };
		}

		exports_range_t<exports_iterator_t> exports()
		{
			const data_directory_t data_directory = this->nt_headers()->optional_header.data_directories.export_directory;

			if (!data_directory.present())
			{
				return { };
			}

			auto module = reinterpret_cast<std::uint8_t*>(this);

			const auto export_directory = reinterpret_cast<export_directory_t*>(module + data_directory.virtual_address);

			auto names = reinterpret_cast<std::uint32_t*>(module + export_directory->address_of_names);
			auto functions = reinterpret_cast<std::uint32_t*>(module + export_directory->address_of_functions);
			auto ordinals = reinterpret_cast<std::uint16_t*>(module + export_directory->address_of_name_ordinals);

			return { module, names, functions, ordinals, export_directory->number_of_names };
		}

		[[nodiscard]] exports_range_t<const exports_iterator_t> exports() const
		{
			const data_directory_t data_directory = this->nt_headers()->optional_header.data_directories.export_directory;

			if (!data_directory.present())
			{
				return { };
			}

			auto module = reinterpret_cast<const std::uint8_t*>(this);

			const auto export_directory = reinterpret_cast<const export_directory_t*>(module + data_directory.virtual_address);

			auto names = reinterpret_cast<const std::uint32_t*>(module + export_directory->address_of_names);
			auto functions = reinterpret_cast<const std::uint32_t*>(module + export_directory->address_of_functions);
			auto ordinals = reinterpret_cast<const std::uint16_t*>(module + export_directory->address_of_name_ordinals);

			return { module, names, functions, ordinals, export_directory->number_of_names };
		}

		imports_range_t<imports_iterator_t> imports()
		{
			data_directory_t data_directory = this->nt_headers()->optional_header.data_directories.import_directory;

			if (!data_directory.present())
			{
				return { };
			}

			auto module = reinterpret_cast<std::uint8_t*>(this);

			return { module, data_directory.virtual_address };
		}

		[[nodiscard]] imports_range_t<const imports_iterator_t> imports() const
		{
			data_directory_t data_directory = this->nt_headers()->optional_header.data_directories.import_directory;

			if (!data_directory.present())
			{
				return { };
			}

			auto module = reinterpret_cast<const std::uint8_t*>(this);

			return { module, data_directory.virtual_address };
		}

		relocations_range_t<relocations_iterator_t> relocations()
		{
			data_directory_t data_directory = this->nt_headers()->optional_header.data_directories.basereloc_directory;

			if (!data_directory.present())
			{
				return { };
			}

			auto module = reinterpret_cast<std::uint8_t*>(this);

			return { module, data_directory.virtual_address };
		}

		[[nodiscard]] relocations_range_t<const relocations_iterator_t> relocations() const
		{
			data_directory_t data_directory = this->nt_headers()->optional_header.data_directories.basereloc_directory;

			if (!data_directory.present())
			{
				return { };
			}

			auto module = reinterpret_cast<const std::uint8_t*>(this);

			return { module, data_directory.virtual_address };
		}

		section_header_t* find_section(std::string_view name);

		[[nodiscard]] const section_header_t* find_section(std::string_view name) const;

		[[nodiscard]] std::uint8_t* find_export(std::string_view name) const;

		// IDA signatures
		[[nodiscard]] std::uint8_t* signature_scan(std::string_view signature) const;

		// byte signatures
		[[nodiscard]] std::uint8_t* signature_scan(const std::uint8_t* pattern, std::size_t pattern_size) const;
	};
}