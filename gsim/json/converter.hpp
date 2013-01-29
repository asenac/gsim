// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * converter.hpp
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

#ifndef GSIM_JSON_CONVERTER_HPP
#define GSIM_JSON_CONVERTER_HPP

#include <gsim/json/writer.hpp>
#include <gsim/json/parser.hpp>

#include <sstream>

namespace gsim 
{
namespace json 
{

/**
 * @brief Converts a type to another compliant by their JSON serialization.
 *
 * @tparam Src Source type.
 * @tparam Dst Destination type.
 * @param source Source type instance.
 * @param dest Destination type instance.
 */
template< typename Src, typename Dst >
inline void convert(const Src& source, Dst& dest)
{
    std::ostringstream oss;
    write(oss, source);
    parse(dest, oss.str());
}

} // namespace json
} // namespace gsim

#endif /* GSIM_JSON_CONVERTER_HPP */

