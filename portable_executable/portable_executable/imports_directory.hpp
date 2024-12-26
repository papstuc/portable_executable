#pragma once

#include <cstdint>
#include <string>
#include <iterator>

namespace portable_executable
{
    struct import_descriptor_t
    {
        union
        {
            std::uint32_t characteristics;
            std::uint32_t original_first_thunk;
        } misc;

        std::uint32_t time_date_stamp;
        std::uint32_t forwarder_chain;
        std::uint32_t name;
        std::uint32_t first_thunk;
    };

    struct thunk_data_t
    {
        union
        {
            std::uint64_t forwarder_string;
            std::uint64_t function;
            std::uint64_t address;

            struct
            {
                std::uint64_t ordinal : 16;
                std::uint64_t _reserved0 : 47;
                std::uint64_t is_ordinal : 1;
            };
        };
    };

    struct import_by_name_t
    {
        std::uint16_t hint;
        char name[1];
    };

    struct import_entry_t
    {
        std::string module_name;
        std::string import_name;
        std::uint8_t* address;
    };

    class imports_iterator_t
    {
    private:
        const std::uint8_t* module = nullptr;

        const import_descriptor_t* current_descriptor = nullptr;
        const thunk_data_t* current_thunk = nullptr;
        const thunk_data_t* original_thunk = nullptr;

    public:
        imports_iterator_t(const std::uint8_t* module, const import_descriptor_t* descriptor);

        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = import_entry_t;
        using pointer = value_type*;
        using reference = value_type&;

        value_type operator*() const;

        imports_iterator_t& operator++();

        bool operator==(const imports_iterator_t& other);

        bool operator!=(const imports_iterator_t& other);
    };
    
    template<typename T>
    class imports_range_t
    {
    private:
        using pointer_type = typename std::conditional<std::is_const<T>::value, const std::uint8_t*, std::uint8_t*>::type;
        using import_descriptor_type = typename std::conditional<std::is_const<T>::value, const import_descriptor_t*, import_descriptor_t*>::type;

        pointer_type m_module = nullptr;

        import_descriptor_type m_import_descriptor = nullptr;

    public:
        imports_range_t() = default;

        imports_range_t(pointer_type module, std::uint32_t imports_rva) :
            m_module(module), m_import_descriptor(reinterpret_cast<import_descriptor_type>(module + imports_rva))
        {

        }

        T begin() const
        {
            return { this->m_module, this->m_import_descriptor };
        }

        T end() const
        {
            return { this->m_module, nullptr };
        }
    };
}