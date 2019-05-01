#include "./m19.h"
#include "./m19_encoder.h"
#include <library/msufsort.h>
#include <cstdint>
#include <iostream>
#include <chrono>


//==========================================================================
auto maniscalco::m19_encode
(
    std::uint8_t const * begin,
    std::uint8_t const * end,
    std::size_t numThreads,
    parsing_method parsingMethod
) -> std::vector<std::uint8_t>
{
    // BWT
    std::cout << "[calculating bwt ...]" << (char)13 << std::flush;
    auto sentinelIndex = maniscalco::forward_burrows_wheeler_transform(begin, end, numThreads);
    
#ifdef TEST_16_BIT_SYMBOLS
std::vector<std::uint16_t> widened;
widened.resize(std::distance(begin, end));
int n = 0;
for (auto cur = begin; cur < end; ++cur)
    widended[n++] = *cur;
#endif

    switch (parsingMethod)
    {
        case parsing_method::m19:
        {
            // encoder model initialization
            std::cout << "[initializing m19 model ...]" << (char)13 << std::flush;
            #ifdef TEST_16_BIT_SYMBOLS
            m19_encoder<m19_context_array> encoder(widended.data(), widended.data() + widened.size(), sentinelIndex);
            #else
            m19_encoder<m19_context_array> encoder(begin, end, sentinelIndex);
            #endif
            // encode full context modelling
            encoder.model_contexts();
            return std::vector<std::uint8_t>();
            break;
        }

        case parsing_method::m03:
        {
            // encoder model initialization
            std::cout << "[initializing m03 model ...]" << (char)13 << std::flush;
            #ifdef TEST_16_BIT_SYMBOLS
            m19_encoder<m03_context_array> encoder(widended.data(), widended.data() + widened.size(), sentinelIndex);
            #else
            m19_encoder<m03_context_array> encoder(begin, end, sentinelIndex);
            #endif
            // encode full context modelling
            encoder.model_contexts();
            return std::vector<std::uint8_t>();
            break;
        }

        default:
        {
            // unknown method
            throw 0; //
        }
    }

}


//======================================================================================================================
void maniscalco::m19_decode
(
    std::uint8_t const *,
    std::uint8_t const *,
    std::uint8_t *,
    std::uint8_t *
)
{
}

