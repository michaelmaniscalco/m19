#pragma once

#include <cstdint>
#include <vector>


namespace maniscalco
{

    std::vector<std::uint8_t> m19_encode
    (
        std::uint8_t const *,
        std::uint8_t const *,
        std::size_t
    );

    void m19_decode
    (
        std::uint8_t const *,
        std::uint8_t const *,
        std::uint8_t *,
        std::uint8_t *
    );

} // namespace maniscalco

