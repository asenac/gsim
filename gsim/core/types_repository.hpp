// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * types_repository.hpp
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

#ifndef GSIM_CORE_TYPES_REPOSITORY_HPP
#define GSIM_CORE_TYPES_REPOSITORY_HPP

#include <gsim/core/descriptor_fwd.hpp>
#include <map>

namespace gsim 
{
namespace core 
{

class types_repository
{
protected:

    types_repository();

public:

    typedef std::map< std::string, descriptor_ptr > map_t;
    typedef map_t::const_iterator const_iterator;

    ~types_repository();

    static const types_repository * instance();

    const_iterator begin() const;
    const_iterator end() const;

    descriptor_ptr get_descriptor(const std::string& fqn) const;
};

} // namespace core
} // namespace gsim

#endif /* GSIM_CORE_TYPES_REPOSITORY_HPP */

