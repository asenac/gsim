// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * descriptor_fwd.hpp
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

#ifndef GSIM_CORE_DESCRIPTOR_FWD_HPP
#define GSIM_CORE_DESCRIPTOR_FWD_HPP

#include <boost/shared_ptr.hpp>

namespace gsim 
{
namespace core 
{

class descriptor_base;
class descriptor_group;
class connection_descriptor;

typedef const descriptor_base * 
    descriptor_ptr;

typedef boost::shared_ptr< descriptor_group > 
    descriptor_group_ptr;

typedef boost::shared_ptr< connection_descriptor >
    connection_descriptor_ref;

typedef const connection_descriptor_ref connection_descriptor_ptr;

typedef boost::shared_ptr< descriptor_base > 
    dynamic_descriptor_ptr;

template< typename T >
struct descriptor;

namespace detail 
{

template < typename T, typename Y >
descriptor_base * create_descriptor(
        const T& t_, 
        const Y& y_, 
        descriptor_ptr parent, 
        unsigned int idx);

} // namespace detail

} // namespace core
} // namespace gsim

#endif /* GSIM_CORE_DESCRIPTOR_FWD_HPP */

