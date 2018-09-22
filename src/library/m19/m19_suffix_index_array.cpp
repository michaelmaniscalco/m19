#include "./m19_suffix_index_array.h"


//==============================================================================
maniscalco::m19_suffix_index_array::m19_suffix_index_array
(
    std::uint8_t const * begin,
    std::uint8_t const * end,
    std::size_t sentinelIndex
):
    size_(std::distance(begin, end) + 1),
    data_(new std::uint16_t[size_])
{
    std::uint8_t const * sentinel = begin + sentinelIndex;

    std::int32_t index[0x100] = {};
    auto cur = begin - 1;
    while (++cur != end)
        ++index[*cur];
    std::int32_t total = 1;
    for (auto & e : index)
    {
        auto temp = e;
        e = total;
        total += temp;
    }
    auto curData = data_.get();
    cur = begin - 1;
    std::int32_t count[0x100] = {};
    while (++cur < end)
    {
        if (cur == sentinel)
        {
            *curData++ = 0x8000; // sentinel index
            --cur;
            sentinel = nullptr;
        }
        else
        {
            auto symbol = *cur;
            auto n = index[symbol]++;
            auto c = count[symbol]++;
            std::int32_t shiftMask = -((c & 0x01) == 0);
            n >>= (15 & shiftMask);
            n &= 0x7fff;
            n |= (((c & 0x01) == 0) << 15);
            *curData++ = n;
        }
    }
}

