// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * Types_adapted.hpp
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

#ifndef TYPES_ADAPTED_HPP
#define TYPES_ADAPTED_HPP

#include <gsim/adapted.hpp>
#include "Types.hpp"

GSIM_ADAPT_STRUCT(status::MyStatus, 
        (status::MyEnum, value))

GSIM_ADAPT_ENUM(status::MyEnum, 
        (ME_LITERAL_0, status::ME_LITERAL_0)
        (ME_LITERAL_1, status::ME_LITERAL_1))

#endif /* TYPES_ADAPTED_HPP */

