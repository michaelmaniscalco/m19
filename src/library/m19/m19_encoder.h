#pragma once

#include "./m19.h"
#include "./m03_context_array.h"
#include "./m19_context_array.h"
#include "./m19_run_length_array.h"
#include "./m19_suffix_index_array.h"
#include <memory>
#include <cstdint>
#include <vector>
#include <iostream>
#include <chrono>
#include <algorithm>


namespace maniscalco 
{

    template <typename context_array_type>
    class m19_encoder
    {
    public:

        m19_encoder
        (
            std::uint8_t const *,
            std::uint8_t const *,
            std::uint32_t
        );

        void model_contexts();

    protected:

    private:

        struct context_stat
        {
            context_stat():startIndex_(), parentCount_(0), childCount_(0), contextArray_(),  deferred_(0){}
            std::uint32_t startIndex_[3];
            std::uint32_t parentCount_;
            std::uint32_t childCount_;
            context_array_type contextArray_;
            std::uint32_t deferred_;
        };

        void model_next_order_contexts
        (
            m19_run_length_array::const_iterator,
            std::vector<std::uint32_t> const &
        );

        m19_run_length_array                    runLengthArray_;

        m19_run_length_array::const_iterator    rlaSentinel_;

        m19_suffix_index_array                  symbolIndexArray_;

        std::uint8_t const *                    input_;

        std::size_t                             inputSize_;

        std::uint32_t                           sentinelIndex_;

        std::array<context_stat, 0x101>         contextStats_;

        std::array<context_stat *, 0x102>       activeContextStats_;

        std::size_t                             currentOrder_;

        std::size_t                             completeCount_;

        std::array<std::uint32_t, 0x101>        symbolCount_;
    };

} // namespace maniscalco

