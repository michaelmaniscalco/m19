#include "./m19_run_length_array.h"
#include <limits>


//==============================================================================
template <typename symbol_type>
maniscalco::m19_run_length_array<symbol_type>::m19_run_length_array
(
    symbol_type const * begin,
    symbol_type const * end,
    std::size_t sentinelIndex
):
    size_(std::distance(begin, end) + 1),
    data_(new element_type[size_])
{
    auto cur = begin;
    auto sentinel = begin + sentinelIndex;

    auto curData = data_.get();
    while (cur < sentinel)
    {
        auto start = cur;
        auto c = *cur++;
        auto cEnd = std::min(sentinel, start + std::numeric_limits<std::uint8_t>::max());
        while ((cur < cEnd) && (*cur == c))
            ++cur;
        std::uint32_t runLength = (std::uint32_t)std::distance(start, cur);
        while (runLength > 0)
            *curData++ = {c, runLength--};
    }
    *curData++ = {0, 1};
    while (cur < end)
    {
        auto start = cur;
        auto c = *cur++;
        auto cEnd = std::min(end, start + std::numeric_limits<std::uint8_t>::max());
        while ((cur < cEnd) && (*cur == c))
            ++cur;
        std::uint32_t runLength = (std::uint32_t)std::distance(start, cur);
        while (runLength > 0)
            *curData++ = {c, runLength--};
    }
}


namespace maniscalco
{
    template class m19_run_length_array<std::uint8_t>;
    template class m19_run_length_array<std::uint16_t>;
}

    