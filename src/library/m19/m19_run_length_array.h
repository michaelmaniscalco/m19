#pragma once

#include <cstdint>
#include <memory>


namespace maniscalco
{

    //==========================================================================
    class m19_run_length_array
    {
    public:

        #pragma pack(push, 1)
        struct entry_type
        {
            entry_type();
            entry_type(entry_type const &) = delete;
            entry_type(entry_type &&) = delete;
            entry_type & operator = (entry_type const &) = delete;
            entry_type & operator = (entry_type &&) = delete;
            entry_type & operator = (std::uint32_t);
            std::uint32_t get_size() const;
            operator std::uint32_t() const;
            std::uint8_t value_;
        };
        #pragma pack(pop)


        class iterator
        {
        public:
            iterator(entry_type *);
            iterator operator ++ (int);
            iterator & operator ++();
            iterator & operator += (int);
            iterator operator + (int) const;
            entry_type * operator ->();
            entry_type & operator *();
            bool operator != (iterator const &) const;
            bool operator == (iterator const &) const;
            bool operator < (iterator const &) const;
            bool operator > (iterator const &) const;

        protected:
        private:
            entry_type * entry_;
        };

        class const_iterator
        {
        public:
            const_iterator(entry_type const *);
            const_iterator operator ++ (int);
            const_iterator & operator += (int);
            const_iterator operator + (int) const;
            const_iterator & operator ++();
            const_iterator(iterator);
            entry_type const * operator ->() const;
            entry_type const & operator *() const;
            bool operator != (const_iterator const &) const;
            bool operator == (const_iterator const &) const;
            bool operator < (const_iterator const &) const;
            bool operator > (const_iterator const &) const;
            std::int64_t operator -
            (
                const_iterator const & other
            ) const
            {
                return (entry_ - other.entry_);
            }

        protected:
        private:
            entry_type const * entry_;
        };


        m19_run_length_array
        (
            std::uint8_t const *,
            std::uint8_t const *,
            std::size_t
        );

        iterator begin();

        const_iterator begin() const;

        iterator end();

        const_iterator end() const;

        entry_type & operator []
        (
            std::size_t
        );

        entry_type const & operator []
        (
            std::size_t
        ) const;

    protected:

    private:

        std::size_t const                       size_;

        std::unique_ptr<entry_type []> const    data_;

    }; // class run_length_array

} // namespace maniscalco


//==============================================================================
inline auto maniscalco::m19_run_length_array::begin
(
) -> iterator
{
    return data_.get();
}


//==============================================================================
inline auto maniscalco::m19_run_length_array::end
(
) -> iterator
{
    return (data_.get() + size_);
}


//==============================================================================
inline auto maniscalco::m19_run_length_array::begin
(
) const -> const_iterator
{
    return data_.get();
}


//==============================================================================
inline auto maniscalco::m19_run_length_array::end
(
) const -> const_iterator
{
    return (data_.get() + size_);
}


//==============================================================================
inline maniscalco::m19_run_length_array::iterator::iterator
(
    entry_type * entry
):
    entry_(entry)
{
}


//==============================================================================
inline maniscalco::m19_run_length_array::const_iterator::const_iterator
(
    entry_type const * entry
):
    entry_(entry)
{
}


//==============================================================================
inline maniscalco::m19_run_length_array::entry_type::entry_type
(
):
    value_()
{
}



//==============================================================================
inline auto maniscalco::m19_run_length_array::iterator::operator ++ 
(
    int
) -> iterator
{
    auto copy = *this;
    ++entry_;
    return copy;
}


//==============================================================================
inline auto maniscalco::m19_run_length_array::iterator::operator += 
(
    int addend
) -> iterator &
{
    entry_ += addend;
    return *this;
}


//==============================================================================
inline auto maniscalco::m19_run_length_array::iterator::operator +
(
    int addend
) const -> iterator 
{
    auto ret = *this;
    ret += addend;
    return ret;
}


//==============================================================================
inline auto maniscalco::m19_run_length_array::iterator::operator ++
(
) -> iterator & 
{
    ++entry_;
    return *this;
}


//==============================================================================
inline auto maniscalco::m19_run_length_array::iterator::operator ->
(
) -> entry_type *
{
    return entry_;
}


//==============================================================================
inline auto maniscalco::m19_run_length_array::iterator::operator *
(
) -> entry_type &
{
    return *entry_;
}


//==============================================================================
inline bool maniscalco::m19_run_length_array::iterator::operator !=
(
    iterator const & other
) const
{
    return (entry_ != other.entry_);
}


//==============================================================================
inline bool maniscalco::m19_run_length_array::iterator::operator ==
(
    iterator const & other
) const
{
    return (entry_ == other.entry_);
}


//==============================================================================
inline bool maniscalco::m19_run_length_array::iterator::operator <
(
    iterator const & other
) const
{
    return (entry_ < other.entry_);
}


//==============================================================================
inline bool maniscalco::m19_run_length_array::iterator::operator >
(
    iterator const & other
) const
{
    return (entry_ > other.entry_);
}


//==============================================================================
inline maniscalco::m19_run_length_array::const_iterator::const_iterator
(
    iterator other
):
    entry_(&*other)
{
}


//==============================================================================
inline auto maniscalco::m19_run_length_array::const_iterator::operator ++ 
(
    int
) -> const_iterator
{
    auto copy = *this;
    ++entry_;
    return copy;
}


//==============================================================================
inline auto maniscalco::m19_run_length_array::const_iterator::operator ++
(
) -> const_iterator & 
{
    ++entry_;
    return *this;
}


//==============================================================================
inline auto maniscalco::m19_run_length_array::const_iterator::operator +=
(
    int addend
) -> const_iterator & 
{
    entry_ += addend;
    return *this;
}


//==============================================================================
inline auto maniscalco::m19_run_length_array::const_iterator::operator +
(
    int addend
) const -> const_iterator 
{
    auto ret = *this;
    ret += addend;
    return ret;
}


//==============================================================================
inline auto maniscalco::m19_run_length_array::const_iterator::operator ->
(
) const -> entry_type const *
{
    return entry_;
}


//==============================================================================
inline auto maniscalco::m19_run_length_array::const_iterator::operator *
(
) const -> entry_type const &
{
    return *entry_;
}


//==============================================================================
inline bool maniscalco::m19_run_length_array::const_iterator::operator !=
(
    const_iterator const & other
) const
{
    return (entry_ != other.entry_);
}


//==============================================================================
inline bool maniscalco::m19_run_length_array::const_iterator::operator ==
(
    const_iterator const & other
) const
{
    return (entry_ == other.entry_);
}


//==============================================================================
inline bool maniscalco::m19_run_length_array::const_iterator::operator <
(
    const_iterator const & other
) const
{
    return (entry_ < other.entry_);
}


//==============================================================================
inline bool maniscalco::m19_run_length_array::const_iterator::operator >
(
    const_iterator const & other
) const
{
    return (entry_ > other.entry_);
}


//==============================================================================
inline auto maniscalco::m19_run_length_array::operator []
(
    std::size_t index
) -> entry_type &
{
    return data_[index];
}


//==============================================================================
inline auto maniscalco::m19_run_length_array::operator []
(
    std::size_t index
) const -> entry_type const &
{
    return data_[index];
}


//==============================================================================
inline std::uint32_t maniscalco::m19_run_length_array::entry_type::get_size
(
) const
{
    auto address = &value_;
    while ((*address & 0xc0) == 0)
        --address;
    auto walkBack = (&value_ - address);
    if (*address & 0x80)
        return ((*address & 0x3f) - walkBack);
    std::size_t result = *(std::uint32_t const *)address;
    return (((result & 0x3f) | ((result & 0x3f00) >> 2) | ((result & 0x3f0000) >> 4) | ((result & 0x3f000000) >> 6)) - walkBack);
}


//==============================================================================
inline maniscalco::m19_run_length_array::entry_type::operator std::uint32_t 
(
) const 
{
    return get_size();
}

