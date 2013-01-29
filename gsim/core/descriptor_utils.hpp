// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * descriptor_utils.hpp
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

#ifndef GSIM_CORE_DESCRIPTOR_UTILS_HPP
#define GSIM_CORE_DESCRIPTOR_UTILS_HPP

#include <gsim/core/descriptor.hpp>
#include <gsim/core/holder.hpp>

namespace gsim 
{
namespace core 
{
namespace utils 
{

/**
 * @brief Tries to copy as much as possible.
 *
 * @param src
 * @param dst
 */
void reflective_copy(holder src, holder dst);

template < typename Src, typename Dst >
void reflective_copy(const Src& s, Dst& d)
{
    holder sh(create_holder(const_cast< Src& >(s)));
    holder dh(create_holder(d));

    reflective_copy(sh, dh);
}

unsigned int nearest_child(descriptor_ptr descriptor, 
        const std::string& name);

const char * get_enum_literal(holder src);

std::size_t calculate_size(holder h);

void initialize(holder h);

holder create_initialized_holder(descriptor_ptr descriptor);

std::string get_member_fqn(descriptor_ptr descriptor);

} // namespace utils
} // namespace core
} // namespace gsim

#endif /* GSIM_CORE_DESCRIPTOR_UTILS_HPP */

