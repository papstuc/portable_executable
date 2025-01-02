#pragma once

#include <cstdint>
#include <string>
#include <iterator>

namespace portable_executable
{
    enum class relocation_type_t : std::uint16_t
    {
        absolute,
        high,
        low,
        high_low,
        high_adj,
        machine_specific_5,
        reserved,
        machine_specific_7,
        machine_specific_8,
        machine_specific_9,
        dir64,
        null
    };

    struct relocation_entry_descriptor_t
    {
        std::uint16_t offset : 12;
        relocation_type_t type : 4;
    };

    struct raw_relocation_block_descriptor_t
    {
        std::uint32_t virtual_address;
        std::uint32_t size_of_block;
    };

    struct relocation_block_t
    {
        std::uint32_t current_entry_index;
        std::uint32_t max_entry_index;
    };

    struct relocation_entry_t
    {
        relocation_entry_descriptor_t descriptor;
        std::uint32_t virtual_address;
    };

    class relocations_iterator_t
    {
    private:
        const raw_relocation_block_descriptor_t* m_current_raw_relocation_block_descriptor = nullptr;
        relocation_block_t m_current_relocation_block = { };

        const relocation_entry_descriptor_t* m_current_descriptor = nullptr;

        void load_block(const raw_relocation_block_descriptor_t* raw_relocation_block_descriptor);

    public:
        relocations_iterator_t() = default;

        // ReSharper disable once CppNonExplicitConvertingConstructor
        relocations_iterator_t(const raw_relocation_block_descriptor_t* raw_relocation_block_descriptor);

        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = relocation_entry_t;
        using pointer = value_type*;
        using reference = value_type&;

        value_type operator*() const;

        relocations_iterator_t& operator++();

        bool operator==(const relocations_iterator_t& other);

        bool operator!=(const relocations_iterator_t& other);
    };

    template<typename T>
    class relocations_range_t
    {
    private:
        using pointer_type = std::conditional_t<std::is_const_v<T>, const std::uint8_t*, std::uint8_t*>;
        using relocation_descriptor_type = std::conditional_t<std::is_const_v<T>, const raw_relocation_block_descriptor_t*, raw_relocation_block_descriptor_t*>;

        pointer_type m_module = nullptr;

        const raw_relocation_block_descriptor_t* m_raw_relocation_block_descriptor = nullptr;

    public:
        relocations_range_t() = default;

        relocations_range_t(pointer_type module, std::uint32_t relocations_rva) :
            m_module(module), m_raw_relocation_block_descriptor(reinterpret_cast<relocation_descriptor_type>(module + relocations_rva))
        {

        }

        T begin() const
        {
            return { this->m_raw_relocation_block_descriptor };
        }

        // ReSharper disable once CppMemberFunctionMayBeStatic
        T end()
        {
            return { nullptr };
        }
    };
}