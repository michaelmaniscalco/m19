#include "./m19.h"
#include "./m19_context_array.h"
#include "./m19_run_length_array.h"
#include "./m19_suffix_index_array.h"
#include <memory>
#include <cstdint>
#include <iostream>
#include <chrono>
#include <algorithm>


#define M19_VERBOSE


namespace maniscalco 
{

    class m19_encoder
    {
    public:

        m19_encoder
        (
            std::uint8_t const * inputBegin,
            std::uint8_t const * inputEnd,
            std::uint32_t sentinelIndex
        ):
            contextArray_(inputBegin, inputEnd),
            runLengthArray_(inputBegin, inputEnd, sentinelIndex),
            symbolIndexArray_(inputBegin, inputEnd, sentinelIndex),
            input_(inputBegin),
            inputSize_(inputEnd - inputBegin + 1),
            sentinelIndex_(sentinelIndex),
            contextStats_({}),
            currentOrder_(0),
            completeCount_(0),
            symbolList_({}),
            symbolListSize_(0)
        {
            symbolList_[symbolListSize_++] = 0;
            bool symbolEncountered[0x101] = {};
            for (auto inputCurrent = inputBegin; inputCurrent < inputEnd; ++inputCurrent)
            {
                std::uint32_t symbol = (((std::uint32_t)*inputCurrent) + 1);
                if (symbolEncountered[symbol] == false)
                {
                    symbolEncountered[symbol] = true;
                    symbolList_[symbolListSize_++] = symbol;
                }
            }
            std::sort(symbolList_.begin(), symbolList_.begin() + symbolListSize_);
            auto symbolListIter = symbolList_.begin();
            for (auto currentContext = contextArray_.begin(); currentContext != contextArray_.end(); )
            {
                auto & contextStat = contextStats_[*symbolListIter++];
                auto size = currentContext->get_size();
                contextStat.contextRange_ = context_range(currentContext, currentContext + size);
                currentContext += size;
            }
        }

        void model_contexts();

    protected:

    private:

        struct context_info
        {
            std::uint32_t size_;
            m19_context_array::type type_;
        };

        class context_range
        {
        public:


            context_range():read_(), write_(), begin_(), end_(){}


            context_range
            (
                m19_context_array::iterator begin,
                m19_context_array::iterator end
            ):
                read_(begin),
                write_(begin),
                begin_(begin),
                end_(end)
            {
            }


            m19_context_array::iterator get_read() const{return read_;}
            m19_context_array::iterator get_write() const{return write_;}
            m19_context_array::iterator get_end() const{return end_;}
            m19_context_array::iterator get_begin() const{return begin_;}


            context_info read
            (
            )
            {
                auto size = read_->get_size();
                context_info contextInfo{size, read_->get_type()};
                read_ += size;
                return contextInfo;
            }


            void write_child
            (
                std::uint32_t size
            )
            {
                auto existingSize = write_->get_size();
                auto existingContextType = write_->get_type();
                write_->set(size, m19_context_array::type::child);
                write_ += size;
                if (size < existingSize)
                    write_->set(existingSize - size, existingContextType);
            }


            void write_end
            (
                std::uint32_t size
            )
            {
                auto existingSize = write_->get_size();
                auto existingContextType = write_->get_type();
                auto toContextType = (existingContextType == m19_context_array::type::skip) ? 
                        m19_context_array::type::skip : (size < existingSize) ? 
                        m19_context_array::type::child : existingContextType;
                write_->set(size, toContextType);
                write_ += size;
                if (size < existingSize)
                    write_->set(existingSize - size, existingContextType);
            }


            void finalize
            (
            )
            {
                if (write_ != end_)
                    write_->set(end_ - write_, m19_context_array::type::skip);
                write_ = begin_;
                read_ = begin_;
            }


            void advance_to_write_position
            (
                m19_context_array::iterator where
            )
            {
                auto endSafe = std::min(where, read_);
                if (write_ < endSafe)
                {
                    auto skipStart = write_;
                    while (write_ < endSafe)
                        write_ += write_->get_size();
                    std::uint32_t skipSize = (std::min(write_, endSafe) - skipStart);
                    skipStart->set(skipSize, m19_context_array::type::skip);
                    if (write_ > endSafe)
                    {
                        endSafe->set((write_ - endSafe), m19_context_array::type::skip);
                        write_ = endSafe;
                    }
                }

                if ((write_ < where) && (write_->is_skip()))
                {
                    auto skipStart = write_;
                    while ((write_ < where) && (write_->is_skip()))
                        write_ += write_->get_size();
                    skipStart->set((std::min(write_, where) - skipStart), m19_context_array::type::skip);
                    if (write_ > where)
                    {
                        where->set((write_ - where), m19_context_array::type::skip);
                        write_ = where;
                    }
                }
                while (write_ < where)
                    write_ += write_->get_size();
            }

        protected:

        private:

            m19_context_array::iterator read_;

            m19_context_array::iterator write_;

            m19_context_array::iterator begin_;

            m19_context_array::iterator end_;
        };


        struct context_stat
        {
            context_stat():startIndex_(), parentCount_(0), childCount_(0), contextRange_(), deferred_(0){}
            std::uint32_t startIndex_[3];
            std::uint32_t parentCount_;
            std::uint32_t childCount_;
            context_range contextRange_;
            std::uint32_t deferred_;
        };

        void model_next_order_contexts
        (
            m19_run_length_array::const_iterator,
            std::vector<std::uint32_t> const &
        );

        m19_context_array               contextArray_;

        m19_run_length_array            runLengthArray_;

        m19_suffix_index_array          symbolIndexArray_;

        std::uint8_t const *            input_;

        std::size_t                     inputSize_;

        std::uint32_t                   sentinelIndex_;

        std::array<context_stat, 0x101> contextStats_;

        std::size_t                     currentOrder_;

        std::size_t                     completeCount_;

        std::array<std::uint32_t, 0x101> symbolList_;

        std::uint32_t                   symbolListSize_;
    };


    //==========================================================================
    void m19_encoder::model_next_order_contexts
    (
        m19_run_length_array::const_iterator runLengthArrayIter,
        std::vector<std::uint32_t> const & contextLength
    )
    {
        context_stat * parentContextSymbolList[0x101];
        context_stat ** parentContextSymbolListEnd = parentContextSymbolList;
        context_stat * childContextSymbolList[0x101];
        context_stat ** childContextSymbolListEnd = childContextSymbolList;

        auto inputIndex = (runLengthArrayIter - runLengthArray_.begin());
        auto sentinelInput = (inputIndex > sentinelIndex_) ? nullptr : (input_ + sentinelIndex_);
        auto input = input_ + inputIndex - (sentinelInput == nullptr);

        auto maxRunLength = runLengthArrayIter->get_size();
        for (auto contextSize : contextLength)
        {
            completeCount_ += (contextSize == 1);
            while (contextSize)
            {
                auto runLength = std::min(contextSize, maxRunLength);
                runLengthArrayIter += runLength;
                contextSize -= runLength;
                if ((maxRunLength -= runLength) == 0)
                    maxRunLength = runLengthArrayIter->get_size();
                std::uint32_t symbol = (input == sentinelInput) ? sentinelInput = nullptr, 0 : (((std::uint32_t)*input) + 1);
                auto & contextStats = contextStats_[symbol];
                if (contextStats.parentCount_ < 2)
                {
                    auto inputIndex = (input - input_) + (sentinelInput == nullptr);
                    contextStats.startIndex_[contextStats.parentCount_] = inputIndex;
                    contextStats.startIndex_[contextStats.parentCount_ + 1] = (inputIndex + 1);
                }
                if ((contextStats.parentCount_ += runLength) == runLength)
                    *(parentContextSymbolListEnd++) = &contextStats;
                if ((contextStats.childCount_ += runLength) == runLength)
                    *(childContextSymbolListEnd++) = &contextStats;
                input += runLength;
                input -= (symbol == 0);
            }
            while (childContextSymbolListEnd != childContextSymbolList)
            {
                auto & contextStats = **--childContextSymbolListEnd;
                if (contextStats.childCount_ != contextStats.parentCount_)
                {
                    if (contextStats.deferred_ == 0)
                    {
                        auto symbolToIndex = symbolIndexArray_[std::make_pair(contextStats.startIndex_[0], contextStats.startIndex_[1])];
                        contextStats.deferred_ = (contextStats.parentCount_ - contextStats.childCount_);
                        contextStats.contextRange_.advance_to_write_position(contextArray_.begin() + symbolToIndex);
                    }
                    contextStats.contextRange_.write_child(contextStats.deferred_);
                    contextStats.deferred_ = contextStats.childCount_;
                }
                contextStats.childCount_ = 0;
            }
        }
        while (parentContextSymbolListEnd != parentContextSymbolList)
        {
            auto & contextStats = **--parentContextSymbolListEnd;
            if (contextStats.childCount_ != contextStats.parentCount_)
                contextStats.contextRange_.write_end(contextStats.deferred_);
            contextStats.deferred_ = 0;
            contextStats.parentCount_ = 0;
        }
    }


    //==========================================================================
    void m19_encoder::model_contexts
    (
    )
    {
        std::vector<std::uint32_t> contextLength;
        contextLength.reserve(0x10001);
        currentOrder_ = 0;
        completeCount_ = 0;
        std::size_t numContexts = 0;

        for (auto currentContext = contextArray_.begin(); currentContext != contextArray_.end(); )
        {
            numContexts++;
            auto size = currentContext->get_size();
            contextLength.push_back(size);
            currentContext->set(size, m19_context_array::type::skip);
            currentContext += size;
        }
        model_next_order_contexts(runLengthArray_.begin(), contextLength);

        #ifdef M19_VERBOSE
            std::cout << "Order 0 child contexts: " << numContexts << std::endl;
        #endif

        while (completeCount_ < inputSize_)
        {
            for (std::size_t i = 0; i < symbolListSize_; ++i)
                contextStats_[symbolList_[i]].contextRange_.finalize();
            ++currentOrder_;

            numContexts = 0;
            for (std::size_t i = 0; i < symbolListSize_; ++i)
            {
                auto & contextRange = contextStats_[symbolList_[i]].contextRange_;
                while (contextRange.get_read() != contextRange.get_end())
                {
                    auto contextInfo = contextRange.read();
                    while (contextInfo.type_ == m19_context_array::type::skip)
                    {
                        if (contextRange.get_read() >= contextRange.get_end())
                            break;
                        contextInfo = contextRange.read();
                    }
                    if (contextInfo.type_ == m19_context_array::type::child)
                    {
                        auto currentChildContext = contextRange.get_read() - contextInfo.size_;
                        auto rla = runLengthArray_.begin() + (currentChildContext - contextArray_.begin());
                        contextLength.clear();
                        contextLength.push_back(contextInfo.size_);
                        while (currentChildContext->set_type(m19_context_array::type::skip) != m19_context_array::type::skip)
                        {
                            currentChildContext = contextRange.get_read();
                            contextInfo = contextRange.read();
                            contextLength.push_back(contextInfo.size_);
                        }
                        numContexts += contextLength.size();
                        model_next_order_contexts(rla, contextLength);
                    }
                }
            }

            #ifdef M19_VERBOSE
                std::cout << "Order " << currentOrder_ << " child contexts: " << numContexts << std::endl;
            #endif
        }

    }
}


//==========================================================================
auto maniscalco::m19_encode
(
    std::uint8_t const * begin,
    std::uint8_t const * end,
    std::size_t sentinelIndex
) -> std::vector<std::uint8_t>
{
    auto startClock = std::chrono::system_clock::now();
    m19_encoder encoder(begin, end, sentinelIndex);
    auto finishClock = std::chrono::system_clock::now();
    auto elapsedClock = (finishClock - startClock);
    auto msElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedClock).count();
    std::cout << "m19 - model initialization time: " << msElapsed << " ms" << std::endl;

    encoder.model_contexts();
    return std::vector<std::uint8_t>();
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

