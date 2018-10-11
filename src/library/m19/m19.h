#pragma once

#include <cstdint>
#include <vector>


namespace maniscalco
{

    enum class parsing_method : std::int32_t
    {
        m03 = 03,
        m19 = 19
    };

    std::vector<std::uint8_t> m19_encode
    (
        std::uint8_t const *,
        std::uint8_t const *,
        std::size_t,
        parsing_method
    );

    void m19_decode
    (
        std::uint8_t const *,
        std::uint8_t const *,
        std::uint8_t *,
        std::uint8_t *
    );

} // namespace maniscalco

