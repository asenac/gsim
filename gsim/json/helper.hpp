// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * helper.hpp
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

#ifndef GSIM_JSON_HELPER_HPP
#define GSIM_JSON_HELPER_HPP

#include <string>

namespace gsim 
{
namespace json 
{
namespace helper 
{

struct helper_base
{
    virtual void new_double(double d)
    {
        throw "Error!";
    }

    virtual void new_string(const std::string& d)
    {
        throw "Error!";
    }

    virtual void new_bool(bool d)
    {
        throw "Error!";
    }

    virtual void new_null()
    {
        throw "Error!";
    }

    // For structs
    virtual helper_base* new_child(const std::string& name)
    {
        throw "Error!";
    }

    // For arrays
    virtual helper_base* new_child()
    {
        throw "Error!";
    }

    virtual ~helper_base() {}
};

} // namespace helper
} // namespace json
} // namespace gsim

#endif /* GSIM_JSON_HELPER_HPP */

