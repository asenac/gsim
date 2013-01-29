// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * descriptor.ipp
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

#ifndef GSIM_CORE_DETAIL_DESCRIPTOR_IPP
#define GSIM_CORE_DETAIL_DESCRIPTOR_IPP

#include <gsim/core/detail/types_repository.hpp>
#include <gsim/core/detail/descriptor.hpp>
#include <gsim/core/detail/holder.ipp>
#include <gsim/core/copy.hpp>
#include <cstring>
#include <algorithm>

#define GSIM_IMPL_DESCRIPTOR_GET_INSTANCE(clazz_)                           \
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

namespace gsim 
{
namespace core 
{
namespace detail 
{

GSIM_IMPL_DESCRIPTOR_GET_INSTANCE(array_descriptor)
GSIM_IMPL_DESCRIPTOR_GET_INSTANCE(enum_descriptor)
GSIM_IMPL_DESCRIPTOR_GET_INSTANCE(sequence_descriptor)
GSIM_IMPL_DESCRIPTOR_GET_INSTANCE(struct_descriptor)
GSIM_IMPL_DESCRIPTOR_GET_INSTANCE(string_descriptor)
GSIM_IMPL_DESCRIPTOR_GET_INSTANCE(unsupported_type)

// 
// Array descriptor
//
template< typename T >
array_descriptor< T >::array_descriptor(descriptor_ptr parent, 
        unsigned int idx) :
    descriptor_base(parent, idx), m_slice(this, 0)
{
    set_fixed_size(m_slice.is_fixed_size());
}

template< typename T >
bool array_descriptor< T >::is_repeated() const { return true; }

template< typename T >
descriptor_ptr array_descriptor< T >::get_slice() const
{
    return &m_slice;
}

template< typename T >
descriptor_ptr array_descriptor< T >::get_child(unsigned int /*unused*/) const
{
    return &m_slice;
}

template< typename T >
descriptor_type array_descriptor< T >::get_type() const
{
    return TYPE_ARRAY;
}

// Dynamic information

template< typename T >
holder array_descriptor< T >::create_holder() const
{
    return new holder_ref_impl< T >();
}

template< typename T >
unsigned int array_descriptor< T >::get_length(holder const& value) const
{
    return size;
}

template< typename T >
unsigned int array_descriptor< T >::get_children_count() const
{
    return size;
}

template< typename T >
holder array_descriptor< T >::get_child_value(holder& value, 
    unsigned int idx) const
{
    typedef holder_ref_impl< T > parent_impl;

    parent_impl * p = reinterpret_cast< parent_impl * >(
            value.m_impl.get());

    return holder( ::gsim::core::create_holder(p->t_[idx]));
}

template< typename T >
void array_descriptor< T >::copy(holder const& src, holder& dst) const
{
    for (std::size_t i = 0; i < size; i++) 
    {
        holder child_src = get_child_value(const_cast< holder& >(src), i);
        holder child_dst = get_child_value(dst, i);
        m_slice.copy(child_src, child_dst);
    }
}

template< typename T >
std::string array_descriptor< T >::to_string(holder const& value) const
{
    return string_converter< T >::to(
            const_cast< holder& >(value).to_value< T >());
}

template< typename T >
void array_descriptor< T >::from_string(holder& value, const std::string& str) const
{
    string_converter< T >::from(str, value.to_value< T >());
}

// String descriptor
template< typename T >
string_descriptor< T >::string_descriptor(descriptor_ptr parent,
        unsigned int idx) :
    descriptor_base(parent, idx)
{
    set_fixed_size(false);
}

template< typename T >
bool string_descriptor< T >::is_variable_length() const { return true; }

template< typename T >
bool string_descriptor< T >::is_primitive() const       { return true; }

template< typename T >
descriptor_type string_descriptor< T >::get_type() const
{
    return TYPE_STRING;
}

template< typename T >
holder string_descriptor< T >::create_holder() const
{
    return new holder_ref_impl< T >();
}

template< typename T >
std::string string_descriptor< T >::to_string(holder const& value) const
{
    return std::string(const_cast< holder& >(value).to_value< T >());
}

template< typename T >
void string_descriptor< T >::from_string(holder& value, const std::string& str) const
{
    value.to_value< T >() = str.c_str();
}

template< typename T >
void string_descriptor< T >::copy(holder const& src, holder& dst) const
{
    dst.to_value< T >() = const_cast< holder& >(src).to_value< T >();
}

// 
// Sequence descriptor
//
template< typename T >
sequence_descriptor< T >::sequence_descriptor(
        descriptor_ptr parent,
        unsigned int idx) :
    descriptor_base(parent, idx)
{
    set_fixed_size(false);

    // Type used to calculate
    slice_t t;

    // Real one
    T y;
    y.resize(1);

    m_slice = 
        ::gsim::core::detail::create_descriptor(
                t, y[0], this, 0);
}

template< typename T >
sequence_descriptor< T >::~sequence_descriptor()
{
    delete m_slice;
}

template< typename T >
descriptor_ptr sequence_descriptor< T >::get_child(unsigned int /*unused*/) const
{
    return m_slice;
}

template< typename T >
bool sequence_descriptor< T >::is_repeated() const        { return true; }

template< typename T >
bool sequence_descriptor< T >::is_variable_length() const { return true; }

template< typename T >
descriptor_type sequence_descriptor< T >::get_type() const
{
    return TYPE_SEQUENCE;
}

template< typename T >
descriptor_ptr sequence_descriptor< T >::get_slice() const
{
    return m_slice;
}

// Dynamic information
template< typename T >
holder sequence_descriptor< T >::create_holder() const
{
    return new holder_ref_impl< T >();
}

template< typename T >
unsigned int sequence_descriptor< T >::get_length(holder const& value) const
{
    typedef holder_ref_impl< T > parent_impl;

    parent_impl const * p = reinterpret_cast< parent_impl const * >(
            value.m_impl.get());

    return p->t_.size();
}

template< typename T >
void sequence_descriptor< T >::set_length(holder& value, 
        unsigned int length) const
{
    typedef holder_ref_impl< T > parent_impl;

    parent_impl const * p = reinterpret_cast< parent_impl const * >(
            value.m_impl.get());

    p->t_.resize(length);
}

template< typename T >
holder sequence_descriptor< T >::get_child_value(holder& value, 
    unsigned int idx) const
{
    typedef holder_ref_impl< T > parent_impl;

    parent_impl * p = reinterpret_cast< parent_impl * >(
            value.m_impl.get());

    return holder( ::gsim::core::create_holder(p->t_[idx]));
}

template< typename T >
void sequence_descriptor< T >::copy(holder const& src, holder& dst) const
{
    unsigned int length = get_length(src);
    set_length(dst, length);

    for (std::size_t i = 0; i < length; i++) 
    {
        holder child_src = get_child_value(const_cast< holder& >(src), i);
        holder child_dst = get_child_value(dst, i);
        m_slice->copy(child_src, child_dst);
    }
}

// Struct descriptor
template< typename S, typename N >
struct accessor : public accessor_base
{
    typedef typename empl::type_of_member< S, N >::type current_t;

    holder get(holder& parent) const
    {
        S& t_ = parent.to_value< S >();

        return holder( ::gsim::core::create_holder(
                    boost::fusion::at < N >(t_)));
    }

    void set(holder& parent, holder& value) const
    {
        S& t_ = parent.to_value< S >();

        do_copy(boost::fusion::at < N >(t_),
                value.to_value<  current_t >());

        // boost::fusion::at < N >(t_) = value.to_value<  current_t >();
    }
};

template < typename S, typename Reflective >
struct create_iterator
{
    Reflective * m_this;

    create_iterator(Reflective * _this) : m_this(_this)
    {}

    template < typename N >
    void operator()(N const& nn)
    {
        typedef 
            ::gsim::adapted::member_helper< S, N::value > 
            helper_t;
        typedef 
            ::gsim::adapted::struct_member< S, N::value > 
            member_t;

        // Tipo que contiene el nombre del campo actual
        typedef empl::name_of_member< S, N > name_t;

        dynamic_descriptor_ptr ptr_(
                helper_t::create_descriptor(m_this));
        ptr_->set_type_fqn(member_t::fqn());
        accessor_ptr ac_(new accessor< S, N >());

        m_this->m_children.push_back(ptr_);
        m_this->m_child_names.push_back(name_t::call());
        m_this->m_accessors.push_back(ac_);
        m_this->m_children_by_name.insert(std::make_pair(name_t::call(), N::value));

        // Registra el tipo en el repositorio
        types_repository::instance()->register_type(member_t::fqn(),
                                                    ptr_.get());
    }
};

//
// Struct descriptor
//

template< typename T >
struct_descriptor< T >::struct_descriptor(
        descriptor_ptr parent, 
        unsigned int idx) :
    descriptor_base(parent, idx)
{
    set_type_fqn( ::gsim::adapted::struct_helper< T >::fqn());

    // Reserve
    m_children.reserve(members_count);
    m_child_names.reserve(members_count);
    m_accessors.reserve(members_count);

    // Iterate
    create_iterator< T, struct_descriptor > it(this);
    boost::mpl::for_each< members_range_t >(it);

    // Calcula si es un tipo de tamaño fijo o variable
    bool fixed = true;

    for (unsigned int i = 0; i < members_count && fixed; i++)
    {
        fixed = m_children[i]->is_fixed_size();
    }

    set_fixed_size(fixed);
}

template< typename T >
unsigned int struct_descriptor< T >::get_children_count() const 
{ 
    return members_count;
}

template< typename T >
const char * struct_descriptor< T >::get_child_name(
        unsigned int idx) const 
{
    return m_child_names[idx];
}

template< typename T >
descriptor_ptr struct_descriptor< T >::get_child(
        unsigned int idx) const
{
    return m_children[idx].get();
}

template< typename T >
descriptor_ptr struct_descriptor< T >::get_child_by_name(const char * name) const
{
    children_by_name_t::const_iterator it = m_children_by_name.find(name);

    if (it != m_children_by_name.end() && it->second < m_children.size())
    {
        return m_children[it->second].get();
    }

    return NULL;
}

template< typename T >
unsigned int struct_descriptor< T >::get_child_index(const char * name) const
{
    children_by_name_t::const_iterator it = m_children_by_name.find(name);

    if (it != m_children_by_name.end())
    {
        return it->second;
    }

    return get_children_count();
}

template< typename T >
descriptor_type struct_descriptor< T >::get_type() const
{
    return TYPE_STRUCT;
}

template< typename T >
void struct_descriptor< T >::copy(holder const& src, holder& dst) const
{
    for (std::size_t i = 0; i < members_count; i++) 
    {
        holder child_src = get_child_value(
                const_cast< holder& >(src), i);

        holder child_dst = get_child_value(dst, i);

        get_child(i)->copy(child_src, child_dst);
    }
}

// Dynamic information
template< typename T >
holder struct_descriptor< T >::create_holder() const
{
    return ::gsim::core::create_holder< T >();
}

template< typename T >
holder struct_descriptor< T >::get_child_value(holder& value, 
    unsigned int idx) const
{
    return m_accessors[idx]->get(value);
}

//
// Enum descriptor
//
template< typename T >
enum_descriptor< T >::enum_descriptor(descriptor_ptr parent,
        unsigned int idx) :
    descriptor_base(parent, idx)
{
}

template< typename T >
bool enum_descriptor< T >::is_enum() const
{
    return true;
}

template< typename T >
descriptor_type enum_descriptor< T >::get_type() const
{
    return TYPE_ENUM;
}

template< typename T >
unsigned int enum_descriptor< T >::get_children_count() const
{
    return adapted_t::size;
}

template< typename T >
const char * enum_descriptor< T >::get_child_name(unsigned int idx) const
{
    return adapted_t::value()[idx].name;
}

template< typename T >
holder enum_descriptor< T >::get_child_value(holder& value, unsigned int idx) const
{
    holder h(create_holder());
    h.to_value< T >() = adapted_t::value()[idx].value;
    return h;
}

template< typename T >
unsigned int enum_descriptor< T >::get_child_index(const char * name) const
{
    unsigned int i = 0;

    for (; i < adapted_t::size; i++) 
    {
        if (std::strcmp(name, adapted_t::value()[i].name) == 0)
        {
            return i;
        }
    }

    return i;
}

template< typename T >
holder enum_descriptor< T >::create_holder() const
{
    return new holder_ref_impl< T >();
}

template< typename T >
void enum_descriptor< T >::copy(holder const& src, holder& dst) const
{
    dst.to_value< T >() = const_cast< holder& >(src).to_value< T >();
}

// 
// Unsupported type
//
template< typename T >
unsupported_type< T >::unsupported_type(descriptor_ptr parent, 
        unsigned int idx) :
    descriptor_base(parent, idx)
{
}

} // namespace detail
} // namespace core
} // namespace gsim

#undef GSIM_IMPL_DESCRIPTOR_GET_INSTANCE

#endif /* GSIM_CORE_DETAIL_DESCRIPTOR_IPP */

