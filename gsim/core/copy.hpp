// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * copy.hpp
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

#ifndef GSIM_CORE_COPY_HPP
#define GSIM_CORE_COPY_HPP

namespace gsim 
{
namespace core 
{

template< typename T >
struct copy
{
    static inline void invoke(const T& src, T& dst)
    {
        dst = src;
    }
};

template< typename T, unsigned int N >
struct copy< T[N] >
{
    typedef T type[N];

    static inline void invoke(const type src, type dst)
    {
        for (unsigned int i = 0; i < N; i++) 
            copy< T >::invoke(src[i], dst[i]);
    }
};

template< typename T >
void do_copy(const T& src, T& dst)
{
    copy< T >::invoke(src, dst);
}

} // namespace core
} // namespace gsim

#endif /* GSIM_CORE_COPY_HPP */

