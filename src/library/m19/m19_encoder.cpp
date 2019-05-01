#include "./m19_encoder.h"


//==============================================================================
template <typename context_array_type>
maniscalco::m19_encoder<context_array_type>::m19_encoder
(
    symbol_type const * inputBegin,
    symbol_type const * inputEnd,
    std::uint32_t sentinelIndex
):
    runLengthArray_(inputBegin, inputEnd, sentinelIndex),
    symbolIndexArray_(inputBegin, inputEnd, sentinelIndex),
    input_(inputBegin),
    inputSize_(inputEnd - inputBegin + 1),
    sentinelIndex_(sentinelIndex),
    contextStats_({}),
    activeContextStats_({}),
    currentOrder_(0),
    completeCount_(0),
    symbolCount_({})
{
    std::uint32_t maxSymbol = *inputBegin;
    for (auto cur = inputBegin; cur != inputEnd; ++cur)
    {
        std::uint32_t symbol = *cur;
        ++symbol;
        if (symbol > maxSymbol)
            maxSymbol = symbol;
        ++symbolCount_[symbol];
    }
    symbolCount_[0] = 1; // sentinel
    std::uint32_t totalCount = 0;
    auto activeContextStats = activeContextStats_.data();
    for (std::uint32_t symbol = 0; symbol <= maxSymbol; ++symbol)
        if (symbolCount_[symbol] != 0)
        {
            contextStats_[symbol].contextArray_ = context_array_type(symbolCount_[symbol], totalCount);
            *activeContextStats++ = (contextStats_.data() + symbol);
            totalCount += symbolCount_[symbol];
        }
    rlaSentinel_ = runLengthArray_.begin() + sentinelIndex;
    *activeContextStats = nullptr;
}


//==============================================================================
template <typename context_array_type>
void maniscalco::m19_encoder<context_array_type>::model_next_order_contexts
(
    m19_run_length_array<symbol_type>::const_iterator runLengthArrayIter,
    std::vector<std::uint32_t> const & contextSizes
)
{
    context_stat * parentContextSymbolList[0x101];
    context_stat ** parentContextSymbolListEnd = parentContextSymbolList;
    context_stat * childContextSymbolList[0x101];
    context_stat ** childContextSymbolListEnd = childContextSymbolList;

    for (auto contextSize : contextSizes)
    {
        completeCount_ += (contextSize == 1);
        auto endRunLengthArrayIter = (runLengthArrayIter + contextSize);
        auto * curContextStats = contextStats_.data(); 
        while (runLengthArrayIter < endRunLengthArrayIter)
        {
            auto symbol = runLengthArrayIter->first;
            auto runLength = runLengthArrayIter->second;
            curContextStats = (contextStats_.data() + symbol);
            curContextStats += (runLengthArrayIter != rlaSentinel_);
            if (curContextStats->parentCount_ < 2)
            {
                auto inputIndex = (runLengthArrayIter - runLengthArray_.begin());
                curContextStats->startIndex_[curContextStats->parentCount_] = inputIndex;
                curContextStats->startIndex_[curContextStats->parentCount_ + 1] = (inputIndex + 1);
            }
            runLengthArrayIter += runLength;
            if ((curContextStats->parentCount_ += runLength) == runLength)
                *(parentContextSymbolListEnd++) = curContextStats;
            if ((curContextStats->childCount_ += runLength) == runLength)
                *(childContextSymbolListEnd++) = curContextStats;
        }
        if (runLengthArrayIter > endRunLengthArrayIter)
        {
            auto overRun = (runLengthArrayIter - endRunLengthArrayIter);
            curContextStats->parentCount_ -= overRun;
            curContextStats->childCount_ -= overRun;
            runLengthArrayIter = endRunLengthArrayIter;
        }
        while (childContextSymbolListEnd != childContextSymbolList)
        {
            auto contextStats = *--childContextSymbolListEnd;
            if (contextStats->childCount_ != contextStats->parentCount_)
            {
                if (contextStats->deferred_ == 0)
                {
                    auto symbolToIndex = symbolIndexArray_[std::make_pair(contextStats->startIndex_[0], contextStats->startIndex_[1])];
                    contextStats->contextArray_.push_skip_to(symbolToIndex);
                    contextStats->contextArray_.push_child(contextStats->parentCount_ - contextStats->childCount_);
                }
                else
                {
                    contextStats->contextArray_.push_child(contextStats->deferred_);
                }
                contextStats->deferred_ = contextStats->childCount_;
            }
            contextStats->childCount_ = 0;
        }
    }
    while (parentContextSymbolListEnd != parentContextSymbolList)
    {
        auto contextStats = *--parentContextSymbolListEnd;
        if (contextStats->deferred_)
            contextStats->contextArray_.push_last_child(contextStats->deferred_);
        contextStats->deferred_ = 0;
        contextStats->parentCount_ = 0;
    }
}


//==============================================================================
template <typename context_array_type>
void maniscalco::m19_encoder<context_array_type>::model_contexts
(
)
{
    std::vector<std::uint32_t> contextSizes;
    contextSizes.reserve(0x10001);
    currentOrder_ = 0;
    completeCount_ = 0;
    std::size_t numContexts = 0;

    for (auto symbolCount : symbolCount_)
        if (symbolCount != 0)
        {
            numContexts++;
            contextSizes.push_back(symbolCount);
        }
    model_next_order_contexts(runLengthArray_.begin(), contextSizes);
    contextSizes.clear();

    std::cout << "[processing order " << currentOrder_ << " contexts ...]" << (char)13 << std::flush;

    while (completeCount_ < inputSize_)
    {
        for (auto contextStats : activeContextStats_)
            if (contextStats != nullptr)
                contextStats->contextArray_.finalize();
        ++currentOrder_;
        numContexts = 0;
        for (auto contextStats : activeContextStats_)
        { 
            if (contextStats == nullptr)
                break;
            auto & contextArray = contextStats->contextArray_;
            auto rla = runLengthArray_.begin() + contextArray.get_starting_index();
            while (!contextArray.empty())
            {
                auto value = contextArray.pop();
                if (value.first == context_array_type::type::child)
                {
                    auto r = rla;
                    while (value.first == context_array_type::type::child)
                    {
                        rla += value.second;
                        contextSizes.push_back(value.second);
                        value = contextArray.pop();
                    }
                    contextSizes.push_back(value.second);
                    numContexts += contextSizes.size();
                    model_next_order_contexts(r, contextSizes);
                    contextSizes.clear();
                }
                rla += value.second;
            }
        }
        std::cout << "[processing order " << currentOrder_ << " contexts ...]" << (char)13 << std::flush;
    }
    std::cout << "max order = " << currentOrder_ << "                               " << std::endl;
}


namespace maniscalco
{
    template class m19_encoder<m03_context_array>;
    template class m19_encoder<m19_context_array>;
}

