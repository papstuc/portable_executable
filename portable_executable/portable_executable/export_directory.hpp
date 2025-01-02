#pragma once

#include <string>
#include <iterator>

namespace portable_executable
{
    struct export_entry_t
    {
        std::string name;
        std::uint8_t* address;
    };

    class exports_iterator_t
    {
        const std::uint8_t* m_module = nullptr;
        const std::uint32_t* m_names = nullptr;
        const std::uint32_t* m_functions = nullptr;
        const std::uint16_t* m_ordinals = nullptr;
        std::uint32_t m_index = 0;

    public:
        exports_iterator_t(const std::uint8_t* module, const std::uint32_t* names, const std::uint32_t* functions, const std::uint16_t* ordinals, std::uint32_t index);

        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = export_entry_t;
        using pointer = value_type*;
        using reference = value_type&;

        value_type operator*() const;

        exports_iterator_t& operator++();

        bool operator==(const exports_iterator_t& other) const;

        bool operator!=(const exports_iterator_t& other) const;
    };

    template<typename T>
    class exports_range_t
    {
    private:
        using pointer_type = std::conditional_t<std::is_const_v<T>, const std::uint8_t*, std::uint8_t*>;
        using uint32_pointer = std::conditional_t<std::is_const_v<T>, const std::uint32_t*, std::uint32_t*>;
        using uint16_pointer = std::conditional_t<std::is_const_v<T>, const std::uint16_t*, std::uint16_t*>;

        pointer_type m_module = nullptr;
        uint32_pointer m_names = nullptr;
        uint32_pointer m_functions = nullptr;
        uint16_pointer m_ordinals = nullptr;

        std::uint32_t m_num_exports = 0;

    public:
        exports_range_t() = default;

        exports_range_t(pointer_type module, uint32_pointer names, uint32_pointer functions, uint16_pointer ordinals, const std::uint32_t num_exports) :
            m_module(module), m_names(names), m_functions(functions), m_ordinals(ordinals), m_num_exports(num_exports)
        {

        }

        T begin() const
        {
            return { this->m_module, this->m_names, this->m_functions, this->m_ordinals, 0 };
        }

        T end() const
        {
            return { this->m_module, this->m_names, this->m_functions, this->m_ordinals, this->m_num_exports };
        }
    };

    struct export_directory_t
    {
        std::uint32_t characteristics;
        std::uint32_t time_date_stamp;
        std::uint16_t major_version;
        std::uint16_t minor_version;
        std::uint32_t name;
        std::uint32_t base;
        std::uint32_t number_of_functions;
        std::uint32_t number_of_names;
        std::uint32_t address_of_functions;
        std::uint32_t address_of_names;
        std::uint32_t address_of_name_ordinals;
    };
}