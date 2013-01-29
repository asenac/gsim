// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * descriptor_explicit.ipp
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

#ifndef GSIM_CORE_DETAIL_DESCRIPTOR_EXPLICIT_IPP
#define GSIM_CORE_DETAIL_DESCRIPTOR_EXPLICIT_IPP

#include <gsim/core/detail/descriptor.hpp>
#include <gsim/core/detail/holder.ipp>
#include <gsim/core/copy.hpp>
#include <typeinfo>
#include <boost/lexical_cast.hpp>

#define GSIM_IMPL_DESCRIPTOR_EXPLICIT_GET_INSTANCE(clazz_)                  \
    template< typename T >                                                  \
    descriptor_ptr                                                          \
    clazz_< T >::instance()                                                 \
    {                                                                       \
        static boost::shared_ptr< clazz_ > _instance(                       \
                new clazz_);                                                \
        return _instance.get();                                             \
    }                                                                       \
    template< typename T >                                                  \
    descriptor_ptr clazz_< T >::get_type_descriptor() const                 \
    {                                                                       \
        return instance();                                                  \
    }                                                                       \
    /***/

#define GSIM_IMPL_DESCRIPTOR_CONVERT_STRING(clazz_)                  \
    template < typename T >                                          \
    std::string clazz_< T >::to_string(holder const& h) const        \
    {                                                                \
        return boost::lexical_cast< std::string >(                   \
                const_cast< holder& >(h).to_value< T >());           \
    }                                                                \
    template < typename T >                                          \
    void clazz_< T >::from_string(holder& h, const std::string& str) const          \
    {                                                                               \
        try {                                                                       \
            h.to_value< T >() = boost::lexical_cast< T >(str);                      \
        } catch(...) {}                                                             \
    }                                                                               \
    /***/

namespace gsim 
{
namespace core 
{
namespace detail 
{

GSIM_IMPL_DESCRIPTOR_EXPLICIT_GET_INSTANCE(bool_descriptor)
GSIM_IMPL_DESCRIPTOR_EXPLICIT_GET_INSTANCE(primitive_descriptor)
GSIM_IMPL_DESCRIPTOR_CONVERT_STRING(bool_descriptor)
GSIM_IMPL_DESCRIPTOR_CONVERT_STRING(primitive_descriptor)

template< typename T >
bool_descriptor< T >::bool_descriptor(descriptor_ptr parent, 
        unsigned int idx) :
    descriptor_base(parent, idx)
{
    set_type_fqn(typeid(T).name());
}

template< typename T >
bool bool_descriptor< T >::is_primitive() const { return true; }

template< typename T >
descriptor_type bool_descriptor< T >::get_type() const
{
    return TYPE_BOOL;
}

template< typename T >
holder bool_descriptor< T >::create_holder() const
{
    return new holder_ref_impl< T >();
}

template< typename T >
void bool_descriptor< T >::copy(holder const& src, holder& dst) const
{
    dst.to_value< T >() = const_cast< holder& >(src).to_value< T >();
}

// Primitive

template< typename T >
holder primitive_descriptor< T >::create_holder() const
{
    return new holder_ref_impl< T >();
}

template< typename T >
primitive_descriptor< T >::primitive_descriptor(
        descriptor_ptr parent, 
        unsigned int idx) :
    descriptor_base(parent, idx)
{
    set_type_fqn(typeid(T).name());
}

template< typename T >
bool primitive_descriptor< T >::is_primitive() const 
{ 
    return true; 
}

template< typename T >
double primitive_descriptor< T >::to_double(holder const& value) const
{
    typedef holder_ref_impl< T > impl_t;

    impl_t const * p = reinterpret_cast< impl_t const * >(
            value.m_impl.get());

    return (double) p->t_;
}

template< >
descriptor_type primitive_descriptor< double >::get_type() const
{
    return TYPE_DOUBLE;
}

template< >
descriptor_type primitive_descriptor< float >::get_type() const
{
    return TYPE_FLOAT;
}

template< >
descriptor_type primitive_descriptor< char >::get_type() const
{
    return TYPE_CHAR;
}

template< >
descriptor_type primitive_descriptor< unsigned char >::get_type() const
{
    return TYPE_OCTET;
}

template< >
descriptor_type primitive_descriptor< short >::get_type() const
{
    return TYPE_SHORT;
}

template< >
descriptor_type primitive_descriptor< unsigned short >::get_type() const
{
    return TYPE_USHORT;
}

template< >
descriptor_type primitive_descriptor< uint32_t >::get_type() const
{
    return TYPE_ULONG;
}

template< >
descriptor_type primitive_descriptor< int32_t >::get_type() const
{
    return TYPE_LONG;
}

template< >
descriptor_type primitive_descriptor< uint64_t >::get_type() const
{
    return TYPE_ULONGLONG;
}

template< >
descriptor_type primitive_descriptor< int64_t >::get_type() const
{
    return TYPE_LONGLONG;
}

template< typename T >
void primitive_descriptor< T >::copy(holder const& src, holder& dst) const
{
    dst.to_value< T >() = const_cast< holder& >(src).to_value< T >();
}

} // namespace detail
} // namespace core
} // namespace gsim

#undef GSIM_IMPL_DESCRIPTOR_EXPLICIT_GET_INSTANCE

#endif /* GSIM_CORE_DETAIL_DESCRIPTOR_EXPLICIT_IPP */

