#include "./m03_context_array.h"


//==============================================================================
maniscalco::m03_context_array::entry_type::entry_type
(
):
    value_(0)
{
}


//==============================================================================
maniscalco::m03_context_array::m03_context_array
(
):
    size_(),
    data_(),
    endData_(),
    startingIndex_(),
    writeIndex_(),
    read_(), 
    write_(),
    readEnd_()
{
}


//==============================================================================
maniscalco::m03_context_array::m03_context_array
(
    std::size_t size,
    std::uint32_t startingIndex
):
    size_(size),
    data_(new entry_type[size_ + 1024 + 8]),
    endData_(data_.get() + size_ + 1024),
    startingIndex_(startingIndex),
    writeIndex_(0),
    read_(data_.get()), 
    write_(read_),
    readEnd_(read_)
{
}


//==============================================================================
maniscalco::m03_context_array::m03_context_array
(
    m03_context_array && other
):
    size_(other.size_),
    data_(std::move(other.data_)),
    endData_(other.endData_),
    startingIndex_(other.startingIndex_),
    writeIndex_(other.writeIndex_),
    read_(other.read_), 
    write_(other.write_),
    readEnd_(other.read_)
{
}


//==============================================================================
auto maniscalco::m03_context_array::operator =
(
    m03_context_array && other
) -> m03_context_array &
{
    size_ = other.size_;
    data_ = std::move(other.data_);
    endData_ = other.endData_;
    startingIndex_ = other.startingIndex_;
    writeIndex_ = other.writeIndex_;
    read_ = other.read_; 
    write_ = other.write_;
    readEnd_ = other.readEnd_;
    return *this;
}

