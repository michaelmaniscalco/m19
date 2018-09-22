#include "./m19_run_length_array.h"


//==============================================================================
auto maniscalco::m19_run_length_array::entry_type::operator =
(
    std::uint32_t input
) -> entry_type &
{
    if (input >= 0x3f)
    {
        std::uint32_t r = ((input & 0x3f) | ((input & 0xfc0) << 2) | ((input & 0x3f000) << 4) | ((input & 0xfc0000) << 6));
        *(std::uint32_t *)&value_ = r;
        value_ |= 0x40;
    }
    else
    {
        value_ = (0x80 | input);
    }
    return *this;
}


//==============================================================================
maniscalco::m19_run_length_array::m19_run_length_array
(
    std::uint8_t const * begin,
    std::uint8_t const * end,
    std::size_t sentinelIndex
):
    size_(std::distance(begin, end) + 1),
    data_(new entry_type[size_ + 4])
{
    auto cur = begin;
    auto sentinel = begin + sentinelIndex;

    auto curData = data_.get();
    while (cur < sentinel)
    {
        auto start = cur;
        auto c = *cur++;
        while ((cur < sentinel) && (*cur == c))
            ++cur;
        std::uint32_t runLength = (std::uint32_t)std::distance(start, cur);
        while (runLength >= 0x3f)
        {
            *curData = runLength;
            curData += 4;
            runLength -= 4; 
        }
        while (runLength > 0)
            *curData++ = runLength--;
    }
    *curData++ = 1;
    while (cur < end)
    {
        auto start = cur;
        auto c = *cur++;
        while ((cur < end) && (*cur == c))
            ++cur;
        std::uint32_t runLength = (std::uint32_t)std::distance(start, cur);
        while (runLength >= 0x3f)
        {
            *curData = runLength;
            curData += 4;
            runLength -= 4; 
        }
        while (runLength > 0)
            *curData++ = runLength--;
    }
}

