#pragma once

#include <cstdint>
#include <memory>


namespace maniscalco
{

    //==========================================================================
    class m19_context_array
    {
    public:

        static constexpr std::uint8_t  type_bits                = 1;
        static constexpr std::uint8_t  long_size_bits           = 2;
        static constexpr std::uint8_t  partial_long_size_bits   = 1;
        static constexpr std::uint8_t  size_bits                = (8 - long_size_bits - partial_long_size_bits - type_bits);

        static constexpr std::uint8_t  type_shift               = size_bits;
        static constexpr std::uint8_t  long_size_flag           = 0x80;
        static constexpr std::uint8_t  short_size_flag          = 0x40;
        static constexpr std::uint8_t  partial_long_size_flag   = 0x20;
        static constexpr std::uint8_t  type_mask                = (((1 << type_bits) - 1) << type_shift);
        static constexpr std::uint8_t  size_mask                = ((1 << size_bits) - 1);
        static constexpr std::uint32_t max_small_size           = size_mask;
        static constexpr std::uint32_t max_short_size           = ((1 << (size_bits << 1)) - 1);
        

        enum type : std::uint8_t
        {
            skip            = 0 << type_shift,
            child           = 1 << type_shift
        };

        #pragma pack(push, 1)
        struct entry_type
        {
            entry_type();
            entry_type(entry_type const & other):value_(other.value_){}
            entry_type & operator = (entry_type const & other){value_ = other.value_; return *this;}
            std::uint32_t get_size() const;
            type get_type() const;
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

        using iterator = entry_type *;

        m19_context_array();

        m19_context_array
        (
            std::size_t,
            std::uint32_t
        );

        m19_context_array
        (
            m19_context_array &&
        );

        m19_context_array & operator =
        (
            m19_context_array &&
        );

        std::uint32_t get_starting_index() const{return startingIndex_;}

        void push_child
        (
            std::uint32_t
        );

        void push_last_child
        (
            std::uint32_t
        );

        void finalize();

        void push_skip_to
        (
            std::uint32_t
        );

        std::pair<type, std::uint32_t> pop();

        bool empty() const;

    protected:

    private:

        std::size_t                     size_;

        std::unique_ptr<entry_type []>  data_;

        std::uint32_t                   startingIndex_;

        iterator                        read_;

        iterator                        write_;

        iterator                        begin_;

        iterator                        end_;

    }; // class m19_context_array

} // namespace maniscalco


//==============================================================================
inline void maniscalco::m19_context_array::push_child
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


//==============================================================================
inline void maniscalco::m19_context_array::push_last_child
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


//==============================================================================
inline void maniscalco::m19_context_array::finalize
(
)
{
    if (write_ < end_)
        write_->set(end_ - write_, m19_context_array::type::skip);
    write_ = begin_;
    read_ = begin_;
}


//==============================================================================
inline void maniscalco::m19_context_array::push_skip_to
(
    std::uint32_t index
)
{
    auto where = begin_ + index;
    if ((write_ < where) && (write_->is_skip()))
    {
        auto endSkip = write_;
        while ((endSkip < where) && (endSkip->is_skip()))
            endSkip += endSkip->get_size();
        write_->set((std::min(endSkip, where) - write_), m19_context_array::type::skip);
        if (where < endSkip)
            where->set((endSkip - where), m19_context_array::type::skip);
    }
    write_ = where;
}


//==============================================================================
inline auto maniscalco::m19_context_array::pop
(
) -> std::pair<type, std::uint32_t>
{
    auto type = read_->set_type(m19_context_array::type::skip);
    auto size = read_->get_size();
    read_ += size;
    return std::make_pair(type, size);
}


//==============================================================================
inline bool maniscalco::m19_context_array::empty
(
) const
{
    return (read_ == end_);
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
        if (size > max_short_size)
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
            value_ = (short_size_flag | contextType | (size >> 4));
            this[1].value_ =  (short_size_flag | partial_long_size_flag | contextType | (size & 0x0f));
        }
    }
    else
    {
        value_ = (contextType | size);
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
    if ((value_ & (long_size_flag | short_size_flag)) == 0)
        return ret;
    if (value_ & short_size_flag)
    {
        this[1].value_ = ((this[1].value_ & ~type_mask) | contextType);
        return ret;
    }
    this[1].value_ = ((this[1].value_ & ~type_mask) | contextType);
    this[2].value_ = ((this[2].value_ & ~type_mask) | contextType);
    this[3].value_ = ((this[3].value_ & ~type_mask) | contextType);
    this[4].value_ = ((this[4].value_ & ~type_mask) | contextType);
    this[5].value_ = ((this[5].value_ & ~type_mask) | contextType);
    this[6].value_ = ((this[6].value_ & ~type_mask) | contextType);
    this[7].value_ = ((this[7].value_ & ~type_mask) | contextType);
    return ret;
}


//==============================================================================
inline std::uint32_t maniscalco::m19_context_array::entry_type::get_size
(
) const
{
    if ((value_ & (long_size_flag | short_size_flag)) == 0)
        return (value_ & size_mask);

    if (value_ & short_size_flag)
    {
        auto current = this;
        std::uint32_t walkBack = ((current->value_ & partial_long_size_flag) == partial_long_size_flag);
        current -= walkBack;
        std::uint32_t size = (current->value_ & size_mask);
        size <<= 4;
        size |= (current[1].value_ & size_mask);
        return (size - walkBack);  
    }

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

