#pragma once

#include "dos_header.hpp"
#include "nt_headers.hpp"

#include "section_header.hpp"
#include "export_directory.hpp"
#include "imports_directory.hpp"
#include "relocations_directory.hpp"

#include <vector>

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

		template <class t>
		t calculate_alignment(t address, t alignment)
		{
			return address + (alignment - (address % alignment));
		}

		std::vector<std::uint8_t> add_section(std::string_view name, std::uint32_t size, std::uint32_t characteristics)
		{
			if (8 < name.size())
			{
				return { };
			}

			const portable_executable::nt_headers_t* nt_headers = this->nt_headers();

			const portable_executable::section_header_t* section_header = this->nt_headers()->section_headers();

			const portable_executable::section_header_t* last_section_header = &section_header[nt_headers->num_sections() - 1];

			portable_executable::section_header_t* new_section_header = reinterpret_cast<portable_executable::section_header_t*>(reinterpret_cast<std::uint64_t>(&last_section_header->characteristics) + 4);

			memcpy(new_section_header, name.data(), name.size());

			new_section_header->virtual_address = calculate_alignment(last_section_header->virtual_address + last_section_header->virtual_size, nt_headers->optional_header.section_alignment);
			new_section_header->virtual_size = calculate_alignment(size + 5, nt_headers->optional_header.section_alignment);
			new_section_header->pointer_to_raw_data = calculate_alignment(last_section_header->pointer_to_raw_data + last_section_header->size_of_raw_data, nt_headers->optional_header.file_alignment);
			new_section_header->size_of_raw_data = calculate_alignment(size + 5, nt_headers->optional_header.file_alignment);
			new_section_header->characteristics = { .flags = characteristics };

			new_section_header->pointer_to_linenumbers = 0;
			new_section_header->pointer_to_relocations = 0;
			new_section_header->number_of_linenumbers = 0;
			new_section_header->number_of_relocations = 0;

			std::vector<std::uint8_t> binary_snapshot = { this->as<const std::uint8_t*>(), this->as<const std::uint8_t*>() + nt_headers->optional_header.size_of_image };

			image_t* new_image = reinterpret_cast<image_t*>(binary_snapshot.data());

			for (portable_executable::section_header_t& section : new_image->sections())
			{
				section.pointer_to_raw_data = section.virtual_address;
			}

			new_image->nt_headers()->optional_header.size_of_image = calculate_alignment(new_image->nt_headers()->optional_header.size_of_image + size + 5 + static_cast<std::uint32_t>(sizeof(portable_executable::section_header_t)), nt_headers->optional_header.section_alignment);
			new_image->nt_headers()->optional_header.size_of_headers = calculate_alignment(new_image->nt_headers()->optional_header.size_of_headers + static_cast<std::uint32_t>(sizeof(portable_executable::section_header_t)), nt_headers->optional_header.file_alignment);
			new_image->nt_headers()->file_header.number_of_sections++;

			binary_snapshot.resize(new_image->nt_headers()->optional_header.size_of_image);

			return binary_snapshot;
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