// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * StringWidget.hpp
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

#ifndef GSIM_QT_STRINGWIDGET_HPP
#define GSIM_QT_STRINGWIDGET_HPP

#include <QtGui>

#include <gsim/qt/export.hpp>
#include <gsim/qt/reflective/ReflectiveWidget.hpp>

namespace gsim 
{
namespace qt 
{

class GSIM_QT_DECLSPEC StringWidget : 
    public QLineEdit, public ReflectiveWidget
{
    Q_OBJECT
    Q_PROPERTY(QString value READ value WRITE setValue)
public:
    StringWidget(TypeDescriptor_ptr reflective,
            QWidget * parent = 0);
    virtual ~StringWidget();

    virtual void toHolder(Holder& holder);
    virtual void fromHolder(Holder& holder);

    void save(QVariant& settings);
    void load(const QVariant& settings);

    QString value() const;
    void setValue(const QString& value);
    
    void _setReadOnly(bool readOnly);
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_STRINGWIDGET_HPP */

