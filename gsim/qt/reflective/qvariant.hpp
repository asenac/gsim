// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * qvariant.hpp
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

#ifndef GSIM_QT_QVARIANT_HPP
#define GSIM_QT_QVARIANT_HPP

#include <gsim/core/descriptor_fwd.hpp>
#include <gsim/core/holder.hpp>
#include <QVariant>

#include <gsim/qt/export.hpp>

namespace gsim 
{
namespace qt
{
    GSIM_QT_DECLSPEC 
    QVariant toQVariant(
            gsim::core::descriptor_ptr descriptor,
            gsim::core::holder& hold);

    GSIM_QT_DECLSPEC 
    bool fromQVariant(
            gsim::core::descriptor_ptr descriptor,
            gsim::core::holder& hold,
            const QVariant& var);

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_QVARIANT_HPP */

