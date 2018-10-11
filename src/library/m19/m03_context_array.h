#pragma once

#include <cstdint>
#include <memory>


namespace maniscalco
{

    //==========================================================================
    class m03_context_array
    {
    public:

        static constexpr std::uint32_t type_shift = 6;
        static constexpr std::uint8_t  type_mask = (1 << type_shift);
        static constexpr std::uint8_t  long_size_flag = 0x80;
        static constexpr std::uint32_t max_small_size = 0x3f;

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
            std::uint32_t set
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

        m03_context_array();

        m03_context_array
        (
            std::size_t,
            std::uint32_t
        );

        m03_context_array
        (
            m03_context_array &&
        );

        m03_context_array & operator =
        (
            m03_context_array &&
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

        iterator                        endData_;

        std::uint32_t                   startingIndex_;

        std::uint32_t                   writeIndex_;

        iterator                        read_;

        iterator                        write_;

        iterator                        readEnd_;

    }; // class m03_context_array

} // namespace maniscalco


//==============================================================================
inline void maniscalco::m03_context_array::push_child
(
    std::uint32_t size
)
{
    writeIndex_ += size;
    write_ += write_->set(size, m03_context_array::type::child);
    if (write_ >= endData_)
        write_ = data_.get();
}


//==============================================================================
inline void maniscalco::m03_context_array::push_last_child
(
    std::uint32_t size
)
{
    writeIndex_ += size;
    write_ += write_->set(size, m03_context_array::type::skip);
    if (write_ >= endData_)
        write_ = data_.get();
}


//==============================================================================
inline void maniscalco::m03_context_array::finalize
(
)
{
    readEnd_ = write_;
    writeIndex_ = 0;
}


//==============================================================================
inline void maniscalco::m03_context_array::push_skip_to
(
    std::uint32_t index
)
{
    if (index > writeIndex_)
    {
        std::uint32_t size = (index - writeIndex_);
        write_ += write_->set(size, m03_context_array::type::skip);
        if (write_ >= endData_)
            write_ = data_.get();
        writeIndex_ = index;
    }
}


//==============================================================================
inline bool maniscalco::m03_context_array::empty
(
) const
{
    return (read_ == readEnd_);
}


//==============================================================================
inline auto maniscalco::m03_context_array::pop
(
) -> std::pair<type, std::uint32_t>
{
    auto size = read_->get_size();
    auto type = read_->get_type();
    read_ += (1 + ((size > max_small_size) << 2));
    if (read_ >= endData_)
        read_ = data_.get();
    return std::make_pair(type, size);
}


//==============================================================================
inline std::uint32_t maniscalco::m03_context_array::entry_type::set
(
    std::uint32_t size,
    type contextType
)
{
    if (size > max_small_size)
    {
        value_ = (contextType | long_size_flag);
        *(std::uint32_t *)(this + 1) = size;
        return 5;
    }
    value_ = (contextType | size);
    return 1;
}


//==============================================================================
inline std::uint32_t maniscalco::m03_context_array::entry_type::get_size
(
) const
{
    if ((value_ & long_size_flag) == 0)
        return (value_ & max_small_size);
    return *(std::uint32_t *)(this + 1);
}


//==============================================================================
inline auto maniscalco::m03_context_array::entry_type::get_type
(
) const -> type
{
    return type(value_ & type_mask);
}

