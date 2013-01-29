// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * tag.hpp
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

#ifndef GSIM_CORE_TAG_HPP
#define GSIM_CORE_TAG_HPP

namespace gsim 
{
namespace core 
{

/**
 * Tags
 */
typedef const int * tag_t;

template< typename T >
struct tag
{
    static inline tag_t value()
    {
        static const int value_ = 0;
        return &value_;
    }
};

} // namespace core
} // namespace gsim

#endif /* GSIM_CORE_TAG_HPP */

