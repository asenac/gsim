// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * IPAddressWidget.hpp
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

#ifndef GSIM_QT_IPADDRESSWIDGET_HPP
#define GSIM_QT_IPADDRESSWIDGET_HPP

#include <QtGui>

#include <gsim/qt/export.hpp>
#include <gsim/qt/reflective/ReflectiveWidget.hpp>
#include <gsim/qt/widgets/IpAddressLineEdit.hpp>

namespace gsim 
{
namespace qt 
{

class GSIM_QT_DECLSPEC IPAddressWidget : 
    public IpAddressLineEdit, public ReflectiveWidget
{
    Q_OBJECT
public:
    IPAddressWidget(TypeDescriptor_ptr reflective,
            QWidget * parent = 0);
    virtual ~IPAddressWidget();

    virtual void toHolder(Holder& holder);
    virtual void fromHolder(Holder& holder);
    
    void save(QVariant& settings);
    void load(const QVariant& settings);

    void _setReadOnly(bool readOnly);

    static QWidget * create(TypeDescriptor_ptr reflective,
            QWidget * parent = 0);

protected:
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_IPADDRESSWIDGET_HPP */

