// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * reflective.hpp
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

#ifndef GSIM_JSON_REFLECTIVE_HPP
#define GSIM_JSON_REFLECTIVE_HPP

#include <ostream>
#include <gsim/core/descriptor_fwd.hpp>
#include <gsim/core/holder.hpp>
#include <gsim/json/helper.hpp>
#include <gsim/json/writer.hpp>

namespace gsim 
{
namespace json 
{

struct reflective_helper : public helper::helper_base
{
    reflective_helper(core::descriptor_ptr reflective,
            core::holder holder);

    virtual ~reflective_helper();

    void new_double(double d);

    void new_string(const std::string& d);

    void new_bool(bool d);

    void new_null();

    // For structs
    helper::helper_base* new_child(const std::string& name);

    // For arrays
    helper::helper_base* new_child();

    core::descriptor_ptr m_reflective;
    core::holder m_holder;
    unsigned int m_currentIndex;
};

typedef gsim::json::writer::json_writer< std::ostream > 
    std_writer_t;

void write(std_writer_t& w, 
        gsim::core::descriptor_ptr reflective, 
        gsim::core::holder holder);

bool parse(core::descriptor_ptr reflective, 
        core::holder& holder, const char * str, size_t size);

bool parse(core::descriptor_ptr reflective, 
        core::holder& holder, std::istream& in);

void write(std::ostream& os, 
        core::descriptor_ptr reflective, 
        core::holder& holder, bool indent = false);

} // namespace json
} // namespace gsim

#endif /* GSIM_JSON_REFLECTIVE_HPP */

