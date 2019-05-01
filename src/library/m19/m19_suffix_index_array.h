#pragma once

#include <cstdint>
#include <memory>


namespace maniscalco
{

    //==========================================================================
    template <typename symbol_type>
    class m19_suffix_index_array
    {
    public:
        
        m19_suffix_index_array
        (
            symbol_type const *,
            symbol_type const *,
            std::size_t
        );

        std::uint32_t operator []
        (
            std::pair<std::uint32_t, std::uint32_t>
        ) const;

    protected:

    private:

        std::size_t const                       size_;

        std::unique_ptr<std::uint16_t []> const data_;

    }; // class m19_suffix_index_array

} // namespace maniscalco


//==============================================================================
template <typename symbol_type>
inline std::uint32_t maniscalco::m19_suffix_index_array<symbol_type>::operator []
(
    std::pair<std::uint32_t, std::uint32_t> index
) const
{
    std::uint32_t msb = data_[index.first];
    std::uint32_t lsb = data_[index.second];
    if ((lsb & 0x8000) == 0x8000)
        return ((((lsb - (msb == 0x7fff)) & 0x7fff) << 15) | msb);
    return (((msb & 0x7fff) << 15) | ((lsb - 1) & 0x7fff));
}

