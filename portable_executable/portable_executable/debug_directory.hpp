#pragma once

#include <cstdint>
#include <string>

namespace portable_executable
{
    enum class debug_directory_type_t : std::uint32_t
    {
        unknown,
        coff,
        codeview,
        fpo,
        misc,
        exception,
        fixup,
        omap_to_src,
        omap_from_src,
        borland,
        _reserved0,
        clsid,
        vc_feature,
        pogo,
        iltcg,
        mpx,
        repro,
        _undefined1,
        _reserved1,
        _undefined2,
        ex_dll_characteristics
    };

    struct debug_directory_t
    {
        std::uint32_t characteristics;
        std::uint32_t time_date_stamp;
        std::uint16_t major_version;
        std::uint16_t minor_version;
        debug_directory_type_t type;
        std::uint32_t size_of_data;
        std::uint32_t virtual_address;
        std::uint32_t pointer_to_raw_data;
    };

    template <typename T>
    class debug_info_iterator_t
    {
    private:
        using pointer_type = std::conditional_t<std::is_const_v<T>, const std::uint8_t*, std::uint8_t*>;

        T* m_debug_info = nullptr;
        T* m_end_debug_info = nullptr;

    public:
        debug_info_iterator_t() = default;

        debug_info_iterator_t(const pointer_type module, const std::uint32_t debug_directory_rva, const std::uint32_t entry_count) :
    		m_debug_info(reinterpret_cast<T*>(module + debug_directory_rva)),
            m_end_debug_info(m_debug_info + entry_count)
        {

        }

        [[nodiscard]] T* begin() const
        {
            return m_debug_info;
        }

        [[nodiscard]] T* end() const
        {
            return m_end_debug_info;
        }
    };
}