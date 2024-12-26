#pragma once

#include <string>
#include <cstdint>
#include <cstddef>

namespace portable_executable
{
    static constexpr std::size_t section_name_size_limit = 8;

    struct section_header_t
    {
        char name[section_name_size_limit];
        std::uint32_t virtual_size;
        std::uint32_t virtual_address;
        std::uint32_t size_of_raw_data;
        std::uint32_t pointer_to_raw_data;
        std::uint32_t pointer_to_relocations;
        std::uint32_t pointer_to_linenumbers;
        std::uint16_t number_of_relocations;
        std::uint16_t number_of_linenumbers;
        std::uint32_t characteristics;

        std::string to_str() const;
    };

    template<typename T>
    class section_iterator_t
    {
    private:
        T* m_base = nullptr;

        std::uint16_t m_num_sections = 0;

    public:
        section_iterator_t(T* base, std::uint16_t num_sections) : m_base(base), m_num_sections(num_sections)
        {

        }

        T* begin() const
        {
            return this->m_base;
        }

        T* end() const
        {
            return this->m_base + this->m_num_sections;
        }
    };
}