// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * properties.cpp
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

#include "properties.hpp"
#include <cstring>
#include <memory>
#include <gsim/core/descriptor_utils.hpp>

using namespace gsim::core;

set_property::set_property(const char * fqn, 
        const char * property,
        const property_t& value)
{
    property_repository::instance()->set_property(
            fqn,
            property,
            value);
}

// property repository

property_repository::property_repository()
{
}

property_repository::~property_repository()
{
}

property_repository * property_repository::instance()
{
    static std::auto_ptr< property_repository > _instance(
            new property_repository);

    return _instance.get();
}

void property_repository::set_property(const char * fqn, 
        const char * property,
        const property_t& value)
{
    m_repository[fqn][property] = value;
}

const property_t * property_repository::get_property(
        const char * fqn, const char * property) const
{
    repository_t::const_iterator map = 
        m_repository.find(fqn);

    if (map != m_repository.end())
    {
        property_map_t::const_iterator it = 
            map->second.find(property);

        if (it != map->second.end())
        {
            return &it->second;
        }
    }

    return NULL;
}

const property_t * property_repository::get_property(
        descriptor_ptr descriptor, const char * property) const
{
    const std::string memberFqn = 
        ::gsim::core::utils::get_member_fqn(descriptor);

    const property_t * p = get_property(memberFqn.c_str(), property);

    if (!p)
    {
        const std::string fqn = descriptor->get_type_fqn();

        p = get_property(fqn.c_str(), property);
    }

    return p;
}

initializer::initializer(initializer_t init)
{
    (*init)();
}

