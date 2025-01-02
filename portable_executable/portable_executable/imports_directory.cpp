#include "imports_directory.hpp"

portable_executable::imports_iterator_t::imports_iterator_t(const std::uint8_t* module, const import_descriptor_t* descriptor) :
	m_module(module), m_current_descriptor(descriptor)
{
    if (this->m_current_descriptor && this->m_current_descriptor->first_thunk)
    {
        this->m_current_thunk = reinterpret_cast<const thunk_data_t*>(this->m_module + m_current_descriptor->first_thunk);
        this->m_original_thunk = reinterpret_cast<const thunk_data_t*>(this->m_module + m_current_descriptor->misc.original_first_thunk);
    }
}

portable_executable::imports_iterator_t::value_type portable_executable::imports_iterator_t::operator*() const
{
    std::string import_name;

    if (this->m_original_thunk->is_ordinal)
    {
        import_name = reinterpret_cast<const char*>(this->m_module + this->m_original_thunk->ordinal);
    }
    else
    {
        const auto import_by_name = reinterpret_cast<const import_by_name_t*>(this->m_module + this->m_original_thunk->address);

        import_name = import_by_name->name;
    }

    const std::string module_name(reinterpret_cast<const char*>(this->m_module + this->m_current_descriptor->name));

    return { module_name, import_name, reinterpret_cast<std::uint8_t*>(this->m_current_thunk->function) };
}

portable_executable::imports_iterator_t& portable_executable::imports_iterator_t::operator++()
{
    if (this->m_current_thunk && this->m_current_thunk->address)
    {
        ++this->m_current_thunk;
        ++this->m_original_thunk;

        if (!this->m_current_thunk->address)
        {
            ++this->m_current_descriptor;

            while (this->m_current_descriptor && this->m_current_descriptor->first_thunk)
            {
                this->m_current_thunk = reinterpret_cast<const thunk_data_t*>(this->m_module + this->m_current_descriptor->first_thunk);
                this->m_original_thunk = reinterpret_cast<const thunk_data_t*>(this->m_module + this->m_current_descriptor->misc.original_first_thunk);

                if (this->m_current_thunk->address)
                {
                    break;
                }

                ++this->m_current_descriptor;
            }

            if (!this->m_current_descriptor || !this->m_current_descriptor->first_thunk)
            {
                this->m_current_descriptor = nullptr;
                this->m_current_thunk = nullptr;
            }
        }
    }

    return *this;
}

bool portable_executable::imports_iterator_t::operator==(const imports_iterator_t& other) const
{
    return this->m_current_descriptor == other.m_current_descriptor && this->m_current_thunk == other.m_current_thunk;
}

bool portable_executable::imports_iterator_t::operator!=(const imports_iterator_t& other) const
{
    return this->m_current_descriptor != other.m_current_descriptor || this->m_current_thunk != other.m_current_thunk;
}
