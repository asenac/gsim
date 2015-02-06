// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * holder.cpp
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

#include "holder.hpp"
#include <gsim/core/detail/holder.ipp>

namespace gsim 
{
namespace core 
{

template bool& holder::to_value< bool >();
template short& holder::to_value< short >();
template unsigned short& holder::to_value< unsigned short >();
template char& holder::to_value< char >();
template unsigned char& holder::to_value< unsigned char >();
template int32_t& holder::to_value< int32_t >();
template uint32_t& holder::to_value< uint32_t >();
template int64_t& holder::to_value< int64_t >();
template uint64_t& holder::to_value< uint64_t >();
template float& holder::to_value< float >();
template double& holder::to_value< double >();

holder_impl_base::~holder_impl_base()
{
}


holder::holder()
{
}

holder::holder(holder_impl_base * impl) :
    m_impl(holder_impl_ptr(impl))
{
}

holder::holder(const holder& o) :
    m_impl(o.m_impl)
{
}

holder& holder::operator=(const holder& o)
{
    m_impl = o.m_impl;

    return *this;
}

descriptor_ptr holder::get_type_descriptor() const
{
    if (!m_impl)
        return NULL;

    return m_impl->get_type_descriptor();
}


holder_impl_base * holder::clone() const
{
    if (!m_impl)
        return NULL;

    return m_impl->clone();
}

bool holder::is_valid() const
{
    return !!m_impl;
}

bool holder::is_reference() const
{
    return m_impl && m_impl->is_reference();
}

std::size_t holder::get_sizeof() const
{
    if (!m_impl)
        return 0;

    return m_impl->get_sizeof();
}

void * holder::get_buffer() const
{
    if (!m_impl)
        return NULL;

    return m_impl->get_buffer();
}

} // namespace core
} // namespace gsim

