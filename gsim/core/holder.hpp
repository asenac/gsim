// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * holder.hpp
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

#ifndef GSIM_CORE_HOLDER_HPP
#define GSIM_CORE_HOLDER_HPP

#include <boost/shared_ptr.hpp>
#include <gsim/core/descriptor_fwd.hpp>

namespace gsim 
{
namespace core 
{

struct holder;

struct holder_impl_base 
{
    virtual ~holder_impl_base();
    virtual holder_impl_base * clone() const = 0;
    virtual descriptor_ptr get_type_descriptor() const = 0;
    virtual bool is_reference() const = 0;
    virtual std::size_t get_sizeof() const = 0;
    virtual void * get_buffer() const = 0;
};

typedef boost::shared_ptr< holder_impl_base > holder_impl_ptr;

template < typename T >
holder_impl_base * create_holder(T& t);

template < typename T >
holder_impl_base * create_holder();

struct holder
{
    holder();

    /**
     * @brief Assumes its shared ownership.
     *
     * @param impl
     */
    holder(holder_impl_base * impl);

    holder(const holder& o);

    holder& operator=(const holder& o);

    template< typename Value >
    Value& to_value();

    holder_impl_base * clone() const;

    bool is_valid() const;
    
    bool is_reference() const;
    
    std::size_t get_sizeof() const;
    
    void * get_buffer() const;

    /**
     * @brief Returns the type descriptor of the contained data.
     *
     * @return The type descriptor instance or NULL if invalid.
     */
    descriptor_ptr get_type_descriptor() const;
    
    holder_impl_ptr m_impl;
};

} // namespace core
} // namespace gsim

#endif /* GSIM_CORE_HOLDER_HPP */

