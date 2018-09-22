#include "./m19_context_array.h"


//==============================================================================
maniscalco::m19_context_array::entry_type::entry_type
(
):
    value_(0)
{
    set(1, type::skip);
}


//==============================================================================
maniscalco::m19_context_array::m19_context_array
(
    std::uint8_t const * begin,
    std::uint8_t const * end
):
    size_(std::distance(begin, end) + 1),
    data_(new entry_type[size_])
{
    std::uint32_t count[0x100] = {};
    std::uint32_t maxSymbol = *begin;
    for (auto cur = begin; cur != end; ++cur)
    {
        if (*cur > maxSymbol)
            maxSymbol = *cur;
        ++count[*cur];
    }
    std::uint32_t index = 1;
    data_[0].set(1, m19_context_array::type::child);
    for (std::uint32_t i = 0; i < 0x100; ++i)
    {
        if (count[i] != 0)
        {
            data_[index].set(count[i], (i == maxSymbol) ? m19_context_array::type::context_end : m19_context_array::type::child);
            index += count[i];
        }
    }
}

