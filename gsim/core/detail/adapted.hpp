// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * adapted.hpp
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

#ifndef GSIM_CORE_DETAIL_ADAPTED_HPP
#define GSIM_CORE_DETAIL_ADAPTED_HPP

#include <gsim/types.hpp>
#include <gsim/core/mpl.hpp>
#include <gsim/core/descriptor_fwd.hpp>

namespace gsim
{
namespace adapted
{

template < typename T >
struct slice;

template < typename T >
struct slice< std::vector< T > >
{
    typedef T type;
};

template< typename Struct, unsigned int member >
struct member_helper
{
    typedef boost::mpl::int_< member > N;
    typedef 
        typename 
        core::empl::type_of_member< Struct, N >::type 
        current_t;

    static inline ::gsim::core::descriptor_base * 
    create_descriptor( ::gsim::core::descriptor_ptr parent)
    {
        Struct s_;
        current_t t_;

        return ::gsim::core::detail::create_descriptor(
                t_, 
                boost::fusion::at < N >(s_) , 
                parent, 
                member);
    }

    template< typename T >
    static inline void set(Struct& s_, const T& t_)
    {
        boost::fusion::at < N >(s_) = t_;
    }

    template< typename T >
    static inline void get(const Struct& s_, T& t_)
    {
        t_ = boost::fusion::at < N >(s_);
    }
};

} // adapted
} // gsim

#endif // GSIM_CORE_DETAIL_ADAPTED_HPP

