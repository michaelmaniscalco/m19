#include "./m19_context_array.h"


//==============================================================================
maniscalco::m19_context_array::entry_type::entry_type
(
):
    value_(0)
{
    set(1, type::skip);
}


//==============================================================================
maniscalco::m19_context_array::m19_context_array
(
):
    size_(),
    data_(),
    startingIndex_(),
    read_(), 
    write_(), 
    begin_(), 
    end_()
{
}


//==============================================================================
maniscalco::m19_context_array::m19_context_array
(
    std::size_t size,
    std::uint32_t startingIndex
):
    size_(size),
    data_(new entry_type[size_]),
    startingIndex_(startingIndex),
    read_(data_.get()), 
    write_(data_.get()), 
    begin_(data_.get()), 
    end_(data_.get() + size_)
{
}


//==============================================================================
maniscalco::m19_context_array::m19_context_array
(
    m19_context_array && other
):
    size_(other.size_),
    data_(std::move(other.data_)),
    startingIndex_(other.startingIndex_),
    read_(other.read_), 
    write_(other.write_), 
    begin_(other.begin_), 
    end_(other.end_)
{
}


//==============================================================================
auto maniscalco::m19_context_array::operator =
(
    m19_context_array && other
) -> m19_context_array &
{
    size_ = other.size_;
    data_ = std::move(other.data_);
    startingIndex_ = other.startingIndex_;
    read_ = other.read_; 
    write_ = other.write_; 
    begin_ = other.begin_; 
    end_ = other.end_;
    return *this;
}

