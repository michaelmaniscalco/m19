#pragma once

#include <cstdint>
#include <memory>


namespace maniscalco
{

    //==========================================================================
    class m19_run_length_array
    {
    public:

        using element_type = std::pair<std::uint8_t, std::uint8_t>;
        using iterator = element_type *;
        using const_iterator = element_type const *;

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

        element_type & operator []
        (
            std::size_t
        );

        element_type operator []
        (
            std::size_t
        ) const;

    protected:

    private:

        std::size_t const                       size_;

        std::unique_ptr<element_type []> const  data_;

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
inline auto maniscalco::m19_run_length_array::operator []
(
    std::size_t index
) -> element_type &
{
    return data_[index];
}


//==============================================================================
inline auto maniscalco::m19_run_length_array::operator []
(
    std::size_t index
) const -> element_type
{
    return data_[index];
}

