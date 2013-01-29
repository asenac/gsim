// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * QVariantHelper.hpp
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

#ifndef GSIM_QT_QVARIANTHELPER_HPP
#define GSIM_QT_QVARIANTHELPER_HPP

#include <QVariant>
#include <boost/shared_ptr.hpp>
#include <gsim/qt/types.hpp>

namespace gsim 
{
namespace qt 
{

class QVariantHelper;
typedef boost::shared_ptr< QVariantHelper > QVariantHelper_ptr;

class QVariantHelper
{
public:
    QVariantHelper();
    virtual ~QVariantHelper();

    virtual QVariant toQVariant(
            TypeDescriptor_ptr descriptor,
            Holder& hold) const = 0;

    virtual bool fromQVariant(
            TypeDescriptor_ptr descriptor,
            Holder& hold,
            const QVariant& var) const = 0;
};

class QVariantHelperRepository
{
public:

    QVariantHelperRepository();
    virtual ~QVariantHelperRepository();

    static QVariantHelperRepository * instance();

    void setHelper(const std::string& fqn, QVariantHelper_ptr helper);

    bool toQVariant(
            TypeDescriptor_ptr descriptor,
            Holder& hold,
            QVariant& var) const;

    bool fromQVariant(
            TypeDescriptor_ptr descriptor,
            Holder& hold,
            const QVariant& var) const;

protected:

    class Data;
    Data * m_data;
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_QVARIANTHELPER_HPP */

