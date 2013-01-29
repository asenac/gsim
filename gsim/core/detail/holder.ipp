// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * holder.ipp
 *
 * GSIM is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSIM is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GSIM_CORE_DETAIL_HOLDER_IPP
#define GSIM_CORE_DETAIL_HOLDER_IPP

#include <gsim/core/holder.hpp>
#include <gsim/core/copy.hpp>
#include <gsim/core/descriptor.hpp>
// Required for instance() method
#include <gsim/core/detail/descriptor.hpp>

namespace gsim 
{
namespace core 
{
namespace detail 
{

template< typename T >
struct holder_ref_impl;

} // namespace detail

template< typename Value >
Value& holder::to_value()
{
    typedef detail::holder_ref_impl< Value > value_impl;

    value_impl * p = reinterpret_cast< value_impl * >(
            m_impl.get());

    return p->t_;
}

namespace detail 
{

template< typename T >
struct holder_ref_impl : public holder_impl_base
{
    typedef T value_type;

    T aux;
    T& t_;

    bool ref;

    holder_ref_impl() : 
        t_(aux), ref(false)
    {
    }

    holder_ref_impl(T& t) : 
        t_(t), ref(true)
    {
    }
/*
    // String sequence case
    holder_ref_impl(const T& t) : 
        aux(t), t_(aux), ref(false)
    {
    }
*/
    ~holder_ref_impl()
    {
    }

    descriptor_ptr get_type_descriptor() const
    {
        return descriptor< T >::instance();
    }

    bool is_reference() const
    {
        return ref;
    }

    std::size_t get_sizeof() const
    {
        return sizeof(T);
    }

    void * get_buffer() const
    {
        return &t_;
    }

    holder_impl_base * clone() const
    {
        // Deep copy
        holder_ref_impl * new_ = 
            new holder_ref_impl;

        do_copy(t_, new_->t_);

        return new_;
    }
};

} // namespace detail

template < typename T >
holder_impl_base * create_holder(T& t)
{
    return new detail::holder_ref_impl< T >(t);
}

template < typename T >
holder_impl_base * create_holder(const T& t)
{
    return new detail::holder_ref_impl< T >(t);
}

template < typename T >
holder_impl_base * create_holder()
{
    return new detail::holder_ref_impl< T >();
}

} // namespace core
} // namespace gsim

#endif /* GSIM_CORE_DETAIL_HOLDER_IPP */

