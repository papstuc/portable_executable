#include "relocations_directory.hpp"

void portable_executable::relocations_iterator_t::load_block(const raw_relocation_block_descriptor_t* raw_relocation_block_descriptor)
{
    this->m_current_raw_relocation_block_descriptor = raw_relocation_block_descriptor;

    if (this->m_current_raw_relocation_block_descriptor && this->m_current_raw_relocation_block_descriptor->virtual_address)
    {
        const std::uint64_t real_block_size = this->m_current_raw_relocation_block_descriptor->size_of_block - sizeof(raw_relocation_block_descriptor_t);

        this->m_current_relocation_block.max_entry_index = static_cast<std::uint32_t>(real_block_size / sizeof(relocation_entry_descriptor_t));
        this->m_current_relocation_block.current_entry_index = 1;

        this->m_current_descriptor = reinterpret_cast<const relocation_entry_descriptor_t*>(this->m_current_raw_relocation_block_descriptor + 1);
    }
}

portable_executable::relocations_iterator_t::relocations_iterator_t(const raw_relocation_block_descriptor_t* raw_relocation_block_descriptor)
{
    if (raw_relocation_block_descriptor)
    {
        this->load_block(raw_relocation_block_descriptor);
    }
}

portable_executable::relocations_iterator_t::value_type portable_executable::relocations_iterator_t::operator*() const
{
    return { *this->m_current_descriptor, this->m_current_raw_relocation_block_descriptor->virtual_address };
}

portable_executable::relocations_iterator_t& portable_executable::relocations_iterator_t::operator++()
{
    if (this->m_current_descriptor && this->m_current_relocation_block.current_entry_index < this->m_current_relocation_block.max_entry_index)
    {
        ++this->m_current_descriptor;

        this->m_current_relocation_block.current_entry_index++;
    }
    else if (this->m_current_raw_relocation_block_descriptor && this->m_current_raw_relocation_block_descriptor->virtual_address)
    {
        this->load_block(reinterpret_cast<const raw_relocation_block_descriptor_t*>(reinterpret_cast<std::uint64_t>(this->m_current_raw_relocation_block_descriptor) + this->m_current_raw_relocation_block_descriptor->size_of_block));
    }
    else
    {
        this->m_current_descriptor = nullptr;
    }

    return *this;
}

bool portable_executable::relocations_iterator_t::operator==(const relocations_iterator_t& other)
{
    return this->m_current_descriptor == other.m_current_descriptor;
}

bool portable_executable::relocations_iterator_t::operator!=(const relocations_iterator_t& other)
{
    return this->m_current_descriptor != other.m_current_descriptor;
}
