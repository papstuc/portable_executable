#pragma once

#include <cstdint>

#include "file_header.hpp"
#include "optional_header.hpp"

#include "section_header.hpp"

namespace portable_executable
{
    struct nt_headers_t
    {
        std::uint32_t signature;
        file_header_t file_header;
        optional_header_t optional_header;

        section_header_t* section_headers();

        const section_header_t* section_headers() const;

        std::uint16_t num_sections() const;
    };
} 