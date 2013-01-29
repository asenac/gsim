// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * path.hpp
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

#ifndef GSIM_QT_REFLECTIVE_PATH_HPP
#define GSIM_QT_REFLECTIVE_PATH_HPP

#include <gsim/qt/types.hpp>
#include <QString>

namespace gsim 
{
namespace qt 
{

QString getFieldName(TypeDescriptor_ptr message,
        const ReflectivePath_t& path);

TypeDescriptor_ptr followPath(
        TypeDescriptor_ptr message,
        const ReflectivePath_t& path);

bool followPath(
        TypeDescriptor_ptr message,
        Holder holder,
        const ReflectivePath_t& path,
        TypeDescriptor_ptr& descriptor,
        Holder& value);

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_REFLECTIVE_PATH_HPP */

