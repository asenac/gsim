// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * writer.hpp
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

#ifndef GSIM_JSON_WRITER_HPP
#define GSIM_JSON_WRITER_HPP

#include <gsim/json/helper.hpp>
#include <gsim/json/detail/json_writer.hpp>
#include <ostream>

namespace gsim 
{
namespace json 
{

template< typename Ostream, typename T >
inline void write(Ostream& os, const T& t, bool indent = false);

template< typename Ostream, typename T >
inline void write(Ostream& os, const T& t, bool indent)
{
    typedef gsim::json::writer::json_writer< Ostream > writer_t;
    writer_t w(os, indent);
    helper::helper_write(w, t);
}

typedef gsim::json::writer::json_writer< std::ostream > ostream_writer_t;

} // namespace json
} // namespace gsim

#endif /* GSIM_JSON_WRITER_HPP */

