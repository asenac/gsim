// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * Types.hpp
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

#ifndef TYPES_HPP
#define TYPES_HPP

#include <gsim/types.hpp>

namespace status 
{
    const gsim::uint32_t ID_STATUS_REQUEST = 1;
    const gsim::uint32_t ID_STATUS_RESPONSE = 2;

    enum MyEnum
    {
        ME_LITERAL_0, ME_LITERAL_1
    };

    struct Header
    {
        gsim::uint32_t id;
        gsim::uint32_t size;
    };

    struct MyRequest
    {
    };

    struct MyStatus
    {
        MyEnum value;
    };

} // namespace status

#endif /* TYPES_HPP */

