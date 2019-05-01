#include "./m19_suffix_index_array.h"


//==============================================================================
template <typename symbol_type>
maniscalco::m19_suffix_index_array<symbol_type>::m19_suffix_index_array
(
    symbol_type const * begin,
    symbol_type const * end,
    std::size_t sentinelIndex
):
    size_(std::distance(begin, end) + 1),
    data_(new std::uint16_t[size_])
{
    symbol_type const * sentinel = begin + sentinelIndex;

    std::uint32_t index[0x100] = {};
    auto curData = data_.get();
    auto cur = begin - 1;
    while (++cur < sentinel)
    {
        auto symbol = *cur;
        auto n = index[symbol]++;
        std::int32_t b = ((n & 1) == 0);
        std::int32_t shiftMask = -b;
        n >>= (15 & shiftMask);
        n &= 0x7fff;
        n |= (b << 15);
        *curData++ = n;
    }

    *curData++ = 0x8000; // sentinel index
    --cur;

    while (++cur < end)
    {
        auto symbol = *cur;
        auto n = index[symbol]++;
        std::int32_t b = ((n & 1) == 0);
        std::int32_t shiftMask = -b;
        n >>= (15 & shiftMask);
        n &= 0x7fff;
        n |= (b << 15);
        *curData++ = n;
    }
}


namespace maniscalco
{
    template class m19_suffix_index_array<std::uint8_t>;
    template class m19_suffix_index_array<std::uint16_t>;
}
