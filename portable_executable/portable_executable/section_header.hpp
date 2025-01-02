#pragma once

#include <string>
#include <cstdint>
#include <cstddef>

namespace portable_executable
{
    static constexpr std::size_t section_name_size_limit = 8;

    union section_characteristics_t
    {
        struct
        {
            std::uint32_t _pad0 : 5;
            std::uint32_t cnt_code : 1;
            std::uint32_t cnt_init_data : 1;
            std::uint32_t cnt_uninit_data : 1;
            std::uint32_t _pad1 : 1;
            std::uint32_t lnk_info : 1;
            std::uint32_t _pad2 : 1;
            std::uint32_t lnk_remove : 1;
            std::uint32_t lnk_comdat : 1;
            std::uint32_t _pad3 : 1;
            std::uint32_t no_defer_spec_exc : 1;
            std::uint32_t mem_far : 1;
            std::uint32_t _pad4 : 1;
            std::uint32_t mem_purgeable : 1;
            std::uint32_t mem_locked : 1;
            std::uint32_t mem_preload : 1;
            std::uint32_t alignment : 4;
            std::uint32_t lnk_nreloc_ovfl : 1;
            std::uint32_t mem_discardable : 1;
            std::uint32_t mem_not_cached : 1;
            std::uint32_t mem_not_paged : 1;
            std::uint32_t mem_shared : 1;
            std::uint32_t mem_execute : 1;
            std::uint32_t mem_read : 1;
            std::uint32_t mem_write : 1;
        };

        std::uint32_t flags;
    };

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
        section_characteristics_t characteristics;

        [[nodiscard]] std::string to_str() const;
    };

    template<typename T>
    class section_iterator_t
    {
    private:
        T* m_base = nullptr;

        std::uint16_t m_num_sections = 0;

    public:
        section_iterator_t(T* base, const std::uint16_t num_sections) : m_base(base), m_num_sections(num_sections)
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