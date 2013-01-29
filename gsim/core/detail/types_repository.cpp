// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * types_repository.cpp
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

#include "types_repository.hpp"
#include <gsim/core/descriptor.hpp>
#include <memory>

using namespace gsim::core::detail;

class types_repository::data
{
public:
    map_t map;
};

types_repository::types_repository() : 
    m_data(new data)
{
}

types_repository::~types_repository()
{
    delete m_data;
}

types_repository * types_repository::instance()
{
    static std::auto_ptr< types_repository  > 
        _instance(new types_repository);

    return _instance.get();
}

types_repository::const_iterator types_repository::begin() const
{
    return m_data->map.begin();
}

types_repository::const_iterator types_repository::end() const
{
    return m_data->map.end();
}

::gsim::core::descriptor_ptr 
types_repository::get_descriptor(const std::string& fqn) const
{
    const_iterator it = m_data->map.find(fqn);

    if (it != m_data->map.end())
        return it->second;

    return NULL;
}

void types_repository::register_type(const char * fqn, 
        descriptor_ptr descriptor)
{
    m_data->map.insert(std::make_pair(fqn, descriptor));
}

