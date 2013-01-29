// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * path.cpp
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

#include "path.hpp"

namespace gsim 
{
namespace qt 
{

QString getFieldName(TypeDescriptor_ptr message,
        const ReflectivePath_t& path)
{
    using namespace gsim::core;

    QString res; 

    descriptor_base const * descriptor = message;

    for (int i = 0; i < path.size(); i++) 
    {
        if (descriptor->get_type() == TYPE_STRUCT)
        {
            res += QString(".") + descriptor->get_child_name(path[i]);

            descriptor = descriptor->get_child(path[i]);
        }
        else if (descriptor->is_repeated())
        {
            res += QString("[%1]").arg(path[i]);
            
            descriptor = descriptor->get_slice();
        }
        else break;
    }

    return res;
}

TypeDescriptor_ptr followPath(
        TypeDescriptor_ptr message,
        const ReflectivePath_t& path)
{
    TypeDescriptor_ptr res = message;

    ReflectivePath_t::const_iterator it = path.begin();

    for (;it != path.end() && res; ++it) 
    {
        res = res->get_child(*it);
    }

    return res;
}

bool followPath(
        TypeDescriptor_ptr message,
        Holder holder,
        const ReflectivePath_t& path,
        TypeDescriptor_ptr& descriptor,
        Holder& value)
{
    value = holder;
    descriptor = message;

    ReflectivePath_t::const_iterator it = path.begin();

    for (;it != path.end() && descriptor; ++it) 
    {
        if (descriptor->is_variable_length())
            return false;

        value = descriptor->get_child_value(value, *it);
        descriptor = descriptor->get_child(*it);
    }

    // valid if descriptor not null
    return (descriptor);
}

QString getMessageName(TypeDescriptor_ptr d)
{
    QString res(d->get_type_name().c_str());
    res = res.toUpper();
    return res;
}

} // namespace qt
} // namespace gsim

