#pragma once

#include <cstdint>
#include <memory>


namespace maniscalco
{

    //==========================================================================
    class m19_context_array
    {
    public:

        static constexpr std::uint8_t  type_bits                = 2;
        static constexpr std::uint8_t  long_size_bits           = 1;
        static constexpr std::uint8_t  partial_long_size_bits   = 1;
        static constexpr std::uint8_t  size_bits                = (8 - long_size_bits - partial_long_size_bits - type_bits);

        static constexpr std::uint8_t  type_shift               = size_bits;
        static constexpr std::uint8_t  long_size_flag           = 0x80;
        static constexpr std::uint8_t  partial_long_size_flag   = 0x40;
        static constexpr std::uint8_t  type_mask                = (((1 << type_bits) - 1) << type_shift);
        static constexpr std::uint8_t  size_mask                = ((1 << size_bits) - 1);
        static constexpr std::uint32_t max_small_size           = size_mask;
        

        enum type : std::uint8_t
        {
            skip            = 0 << type_shift,
            child           = 1 << type_shift,
            context_end     = 2 << type_shift
        };


        #pragma pack(push, 1)
        struct entry_type
        {
            entry_type();
            entry_type(entry_type const & other):value_(other.value_){}
       //     entry_type(entry_type &&) = delete;
            entry_type & operator = (entry_type const & other){value_ = other.value_; return *this;}
       //     entry_type & operator = (entry_type &&) = delete;
            std::uint32_t get_size() const;
            type get_type() const;
            bool is_end() const {return (get_type() == type::context_end);}
            bool is_child() const {return (get_type() == type::child);}
            bool is_skip() const{return (get_type() == type::skip);}
            void set
            (
                std::uint32_t,
                type
            );
            type set_type
            (
                type
            );
            std::uint8_t value_;
        };
        #pragma pack(pop)


        class iterator
        {
        public:
            iterator():entry_(nullptr){}
            iterator(entry_type *);
            iterator operator ++ (int);
            iterator & operator ++ ();
            iterator & operator += (int);
            iterator operator + (int) const;
            entry_type * operator -> ();
            entry_type & operator * ();
            bool operator != (iterator const &) const;
            bool operator == (iterator const &) const;
            bool operator < (iterator const &) const;
            bool operator <= (iterator const &) const;
            bool operator > (iterator const &) const;
            bool operator >= (iterator const &) const;
            std::int64_t operator -
            (
                iterator const & other
            ) const
            {
                return (entry_ - other.entry_);
            }
            iterator operator -
            (
                std::uint32_t n
            ) const
            {
                return  (entry_ - n);
            }
        protected:
        private:
            entry_type * entry_;
        };


        class const_iterator
        {
        public:
            const_iterator():entry_(nullptr){}
            const_iterator(entry_type const *);
            const_iterator operator ++ (int);
            const_iterator & operator ++ ();
            const_iterator & operator += (int);
            const_iterator operator + (int) const;
            const_iterator(iterator);
            entry_type const * operator -> () const;
            entry_type const & operator * () const;
            bool operator != (const_iterator const &) const;
            bool operator == (const_iterator const &) const;
            bool operator < (const_iterator const &) const;
            bool operator <= (const_iterator const &) const;
            bool operator > (const_iterator const &) const;
            bool operator >= (const_iterator const &) const;
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


        m19_context_array
        (
            std::uint8_t const *,
            std::uint8_t const *
        );

        entry_type & operator []
        (
            std::size_t
        );

        entry_type const & operator []
        (
            std::size_t
        ) const;

        iterator begin();

        const_iterator begin() const;

        iterator end();

        const_iterator end() const;

    protected:

    private:

        std::size_t const                       size_;

        std::unique_ptr<entry_type []> const    data_;

    }; // class m19_context_array

} // namespace maniscalco



//==============================================================================
inline auto maniscalco::m19_context_array::operator []
(
    std::size_t index
) -> entry_type &
{
    return data_[index];
}


//==============================================================================
inline auto maniscalco::m19_context_array::operator []
(
    std::size_t index
) const -> entry_type const &
{
    return data_[index];
}


//==============================================================================
inline auto maniscalco::m19_context_array::begin
(
) -> iterator
{
    return data_.get();
}


//==============================================================================
inline auto maniscalco::m19_context_array::end
(
) -> iterator
{
    return (data_.get() + size_);
}


//==============================================================================
inline auto maniscalco::m19_context_array::begin
(
) const -> const_iterator
{
    return data_.get();
}


//==============================================================================
inline auto maniscalco::m19_context_array::end
(
) const -> const_iterator
{
    return (data_.get() + size_);
}


//==============================================================================
inline void maniscalco::m19_context_array::entry_type::set
(
    std::uint32_t size,
    type contextType
)
{
    if (size > max_small_size)
    {
        value_ = (long_size_flag | contextType | ((size & 0xf0000000) >> 28));
        this[1].value_ = (long_size_flag | partial_long_size_flag | contextType | ((size & 0x0f000000) >> 24));
        this[2].value_ = (long_size_flag | partial_long_size_flag | contextType | ((size & 0x00f00000) >> 20));
        this[3].value_ = (long_size_flag | partial_long_size_flag | contextType | ((size & 0x000f0000) >> 16));
        this[4].value_ = (long_size_flag | partial_long_size_flag | contextType | ((size & 0x0000f000) >> 12));
        this[5].value_ = (long_size_flag | partial_long_size_flag | contextType | ((size & 0x00000f00) >> 8));
        this[6].value_ = (long_size_flag | partial_long_size_flag | contextType | ((size & 0x000000f0) >> 4));
        this[7].value_ = (long_size_flag | partial_long_size_flag | contextType | (size & 0x0000000f));
    }
    else
    {
        value_ = (contextType | (size & size_mask));
    }
}


//==============================================================================
inline auto maniscalco::m19_context_array::entry_type::set_type
(
    type contextType
) -> type
{
    type ret = (type)(value_ & type_mask);
    value_ = ((value_ & ~type_mask) | contextType);
    if (value_ & long_size_flag)
    {
        this[1].value_ = ((this[1].value_ & ~type_mask) | contextType);
        this[2].value_ = ((this[2].value_ & ~type_mask) | contextType);
        this[3].value_ = ((this[3].value_ & ~type_mask) | contextType);
        this[4].value_ = ((this[4].value_ & ~type_mask) | contextType);
        this[5].value_ = ((this[5].value_ & ~type_mask) | contextType);
        this[6].value_ = ((this[6].value_ & ~type_mask) | contextType);
        this[7].value_ = ((this[7].value_ & ~type_mask) | contextType);
    }
    return ret;
}


//==============================================================================
inline std::uint32_t maniscalco::m19_context_array::entry_type::get_size
(
) const
{
    if ((value_ & long_size_flag) == 0)
        return (value_ & size_mask);
    
    auto current = this;
    while ((current->value_ & partial_long_size_flag) == partial_long_size_flag)
        --current;
    auto walkBack = (this - current);
    std::uint32_t size = (current->value_ & size_mask);
    size <<= 4;
    size |= (current[1].value_ & size_mask);
    size <<= 4;
    size |= (current[2].value_ & size_mask);
    size <<= 4;
    size |= (current[3].value_ & size_mask);
    size <<= 4;
    size |= (current[4].value_ & size_mask);
    size <<= 4;
    size |= (current[5].value_ & size_mask);
    size <<= 4;
    size |= (current[6].value_ & size_mask);
    size <<= 4;
    size |= (current[7].value_ & size_mask);
    return (size - walkBack);
}


//==============================================================================
inline auto maniscalco::m19_context_array::entry_type::get_type
(
) const -> type
{
    return type(value_ & type_mask);
}


//==============================================================================
inline maniscalco::m19_context_array::iterator::iterator
(
    entry_type * entry
):
    entry_(entry)
{
}


//==============================================================================
inline auto maniscalco::m19_context_array::iterator::operator ++ 
(
    int
) -> iterator
{
    auto copy = *this;
    ++entry_;
    return copy;
}


//==============================================================================
inline auto maniscalco::m19_context_array::iterator::operator += 
(
    int addend
) -> iterator &
{
    entry_ += addend;
    return *this;
}


//==============================================================================
inline auto maniscalco::m19_context_array::iterator::operator + 
(
    int addend
) const -> iterator
{
    auto ret = *this;
    ret.entry_ += addend;
    return ret;
}


//==============================================================================
inline auto maniscalco::m19_context_array::iterator::operator ++
(
) -> iterator &
{
    ++entry_;
    return *this;
}


//==============================================================================
inline auto maniscalco::m19_context_array::iterator::operator ->
(
) -> entry_type * 
{
    return entry_;
}


//==============================================================================
inline auto maniscalco::m19_context_array::iterator::operator *
(
) -> entry_type & 
{
    return *entry_;
}


//==============================================================================
inline bool maniscalco::m19_context_array::iterator::operator !=
(
    iterator const & other
) const
{
    return (entry_ != other.entry_);
}


//==============================================================================
inline bool maniscalco::m19_context_array::iterator::operator ==
(
    iterator const & other
) const
{
    return (entry_ == other.entry_);
}


//==============================================================================
inline bool maniscalco::m19_context_array::iterator::operator <=
(
    iterator const & other
) const
{
    return (entry_ <= other.entry_);
}


//==============================================================================
inline bool maniscalco::m19_context_array::iterator::operator <
(
    iterator const & other
) const
{
    return (entry_ < other.entry_);
}


//==============================================================================
inline bool maniscalco::m19_context_array::iterator::operator >
(
    iterator const & other
) const
{
    return (entry_ > other.entry_);
}


//==============================================================================
inline bool maniscalco::m19_context_array::iterator::operator >=
(
    iterator const & other
) const
{
    return (entry_ >= other.entry_);
}


//==============================================================================
inline maniscalco::m19_context_array::const_iterator::const_iterator
(
    iterator other
):
    entry_(&*other)
{
}


//==============================================================================
inline maniscalco::m19_context_array::const_iterator::const_iterator
(
    entry_type const * entry
):
    entry_(entry)
{
}


//==============================================================================
inline auto maniscalco::m19_context_array::const_iterator::operator ++ 
(
    int
) -> const_iterator
{
    auto copy = *this;
    ++entry_;
    return copy;
}


//==============================================================================
inline auto maniscalco::m19_context_array::const_iterator::operator ++
(
) -> const_iterator &
{
    ++entry_;
    return *this;
}


//==============================================================================
inline auto maniscalco::m19_context_array::const_iterator::operator += 
(
    int addend
) -> const_iterator &
{
    entry_ += addend;
    return *this;
}


//==============================================================================
inline auto maniscalco::m19_context_array::const_iterator::operator + 
(
    int addend
) const -> const_iterator
{
    auto ret = *this;
    ret.entry_ += addend;
    return ret;
}


//==============================================================================
inline auto maniscalco::m19_context_array::const_iterator::operator ->
(
) const -> entry_type const * 
{
    return entry_;
}


//==============================================================================
inline auto maniscalco::m19_context_array::const_iterator::operator *
(
) const -> entry_type const & 
{
    return *entry_;
}


//==============================================================================
inline bool maniscalco::m19_context_array::const_iterator::operator !=
(
    const_iterator const & other
) const
{
    return (entry_ != other.entry_);
}


//==============================================================================
inline bool maniscalco::m19_context_array::const_iterator::operator ==
(
    const_iterator const & other
) const
{
    return (entry_ == other.entry_);
}


//==============================================================================
inline bool maniscalco::m19_context_array::const_iterator::operator <
(
    const_iterator const & other
) const
{
    return (entry_ < other.entry_);
}


//==============================================================================
inline bool maniscalco::m19_context_array::const_iterator::operator <=
(
    const_iterator const & other
) const
{
    return (entry_ <= other.entry_);
}


//==============================================================================
inline bool maniscalco::m19_context_array::const_iterator::operator >
(
    const_iterator const & other
) const
{
    return (entry_ > other.entry_);
}


//==============================================================================
inline bool maniscalco::m19_context_array::const_iterator::operator >=
(
    const_iterator const & other
) const
{
    return (entry_ >= other.entry_);
}

