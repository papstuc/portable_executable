#pragma once

#include <cstdint>

namespace portable_executable
{
    struct file_header_t
    {
        std::uint16_t machine;
        std::uint16_t number_of_sections;
        std::uint32_t time_date_stamp;
        std::uint32_t pointer_to_symbol_table;
        std::uint32_t number_of_symbols;
        std::uint16_t sizeof_optional_header;
        std::uint16_t characteristics;
    };
}
