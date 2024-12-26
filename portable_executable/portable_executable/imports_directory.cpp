#include "imports_directory.hpp"

portable_executable::imports_iterator_t::imports_iterator_t(const std::uint8_t* module, const import_descriptor_t* descriptor) :
	module(module), current_descriptor(descriptor)
{
    if (this->current_descriptor && this->current_descriptor->first_thunk)
    {
        this->current_thunk = reinterpret_cast<const thunk_data_t*>(this->module + current_descriptor->first_thunk);
        this->original_thunk = reinterpret_cast<const thunk_data_t*>(this->module + current_descriptor->misc.original_first_thunk);
    }
}

portable_executable::imports_iterator_t::value_type portable_executable::imports_iterator_t::operator*() const
{
    std::string import_name;

    if (this->original_thunk->is_ordinal)
    {
        import_name = reinterpret_cast<const char*>(this->module + this->original_thunk->ordinal);
    }
    else
    {
        auto import_by_name = reinterpret_cast<const import_by_name_t*>(this->module + this->original_thunk->address);

        import_name = import_by_name->name;
    }

    std::string module_name(reinterpret_cast<const char*>(this->module + this->current_descriptor->name));

    return { module_name, import_name, reinterpret_cast<std::uint8_t*>(this->current_thunk->function) };
}

portable_executable::imports_iterator_t& portable_executable::imports_iterator_t::operator++()
{
    if (this->current_thunk && this->current_thunk->address)
    {
        ++this->current_thunk;
        ++this->original_thunk;

        if (!this->current_thunk->address)
        {
            ++this->current_descriptor;

            while (this->current_descriptor && this->current_descriptor->first_thunk)
            {
                this->current_thunk = reinterpret_cast<const thunk_data_t*>(this->module + this->current_descriptor->first_thunk);
                this->original_thunk = reinterpret_cast<const thunk_data_t*>(this->module + this->current_descriptor->misc.original_first_thunk);

                if (this->current_thunk->address)
                {
                    break;
                }

                ++this->current_descriptor;
            }

            if (!this->current_descriptor || !this->current_descriptor->first_thunk)
            {
                this->current_descriptor = nullptr;
                this->current_thunk = nullptr;
            }
        }
    }

    return *this;
}

bool portable_executable::imports_iterator_t::operator==(const imports_iterator_t& other)
{
    return this->current_descriptor == other.current_descriptor && this->current_thunk == other.current_thunk;
}

bool portable_executable::imports_iterator_t::operator!=(const imports_iterator_t& other)
{
    return this->current_descriptor != other.current_descriptor || this->current_thunk != other.current_thunk;
}
