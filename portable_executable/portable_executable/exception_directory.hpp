#pragma once

#include <cstdint>
#include <string>

namespace portable_executable
{
    class unwind_code_iterator_t;

    struct unwind_code_t
    {
        std::uint8_t offset;
        std::uint8_t code : 4;
        std::uint8_t info : 4;
    };

    struct unwind_info_t
    {
        std::uint8_t version : 3;
        std::uint8_t flags : 5;
        std::uint8_t size_of_prolog;
        std::uint8_t unwind_code_count;
        std::uint8_t frame_register : 4;
        std::uint8_t frame_offset : 4;
        unwind_code_t codes[1];

        [[nodiscard]] unwind_code_iterator_t begin() const;
        [[nodiscard]] unwind_code_iterator_t end() const;
    };

    struct runtime_function_t
    {
        std::uint32_t begin_address;
        std::uint32_t end_address;
        std::uint32_t unwind_info_rva;
    };

    struct runtime_function_descriptor_t
    {
        std::uint8_t* function_begin;
        std::uint8_t* function_end;

        unwind_info_t* unwind_info;
    };

    class unwind_code_iterator_t
    {
        const unwind_code_t* m_current_code = nullptr;

    public:
        unwind_code_iterator_t() = default;

        unwind_code_iterator_t(const unwind_code_t* unwind_code) :
            m_current_code(unwind_code)
        {
            
        }

        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = unwind_code_t;
        using pointer = value_type*;
        using reference = value_type&;

        value_type operator*() const;

        unwind_code_iterator_t& operator++();

        bool operator==(const unwind_code_iterator_t& other) const;
        bool operator!=(const unwind_code_iterator_t& other) const;
    };

    class runtime_functions_iterator_t
    {
        const std::uint8_t* m_module = nullptr;
        const runtime_function_t* m_current_function = nullptr;

    public:
        runtime_functions_iterator_t() = default;

        runtime_functions_iterator_t(const std::uint8_t* const module, const runtime_function_t* runtime_function) :
            m_module(module),
			m_current_function(runtime_function)
        {

        }

        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = runtime_function_descriptor_t;
        using pointer = value_type*;
        using reference = value_type&;

        value_type operator*() const;

        runtime_functions_iterator_t& operator++();

        bool operator==(const runtime_functions_iterator_t& other) const;
        bool operator!=(const runtime_functions_iterator_t& other) const;
    };

    template <typename T>
    class runtime_functions_range_t
    {
    private:
        using pointer_type = std::conditional_t<std::is_const_v<T>, const std::uint8_t*, std::uint8_t*>;
        using runtime_function_type = std::conditional_t<std::is_const_v<T>, const runtime_function_t*, runtime_function_t*>;

        pointer_type m_module = nullptr;

        runtime_function_type m_runtime_functions = nullptr;
        runtime_function_type m_end_runtime_functions = nullptr;

    public:
        runtime_functions_range_t() = default;

        runtime_functions_range_t(const pointer_type module, const std::uint32_t exception_directory_rva, const std::uint32_t exception_directory_size) :
            m_module(module),
            m_runtime_functions(reinterpret_cast<runtime_function_type>(module + exception_directory_rva)),
            m_end_runtime_functions(reinterpret_cast<runtime_function_type>(module + exception_directory_rva + exception_directory_size))
        {

        }

        [[nodiscard]] T begin() const
        {
            return { m_module, m_runtime_functions };
        }

        [[nodiscard]] T end() const
        {
            return { m_module, m_end_runtime_functions };
        }
    };
}