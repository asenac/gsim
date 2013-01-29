// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * LSBWidget.hpp
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

#ifndef GSIM_QT_LSBWIDGET_HPP
#define GSIM_QT_LSBWIDGET_HPP

#include <QtGui>

#include <gsim/qt/export.hpp>
#include <gsim/qt/reflective/FloatWidget.hpp>
#include <gsim/qt/reflective/QVariantHelper.hpp>

namespace gsim 
{
namespace qt 
{

class GSIM_QT_DECLSPEC LSBWidget : 
    public FloatWidget
{
    Q_OBJECT
    Q_PROPERTY(float LSB READ LSB WRITE setLSB)
public:
    LSBWidget(TypeDescriptor_ptr reflective,
            float lsb,
            QWidget * parent = 0);
    virtual ~LSBWidget();

    virtual void toHolder(Holder& holder);
    virtual void fromHolder(Holder& holder);
    
    static QWidget * create(TypeDescriptor_ptr reflective,
            float lsb,
            QWidget * parent = 0);

    float LSB() const;

public slots:

    void setLSB(float lsb);

protected:

    float m_lsb;
};

class LSBHelper : public QVariantHelper
{
public:
    LSBHelper(float lsb);
    ~LSBHelper();

    QVariant toQVariant(
            TypeDescriptor_ptr descriptor,
            Holder& hold) const;

    bool fromQVariant(
            TypeDescriptor_ptr descriptor,
            Holder& hold,
            const QVariant& var) const;

    static QVariantHelper_ptr create(float lsb);

protected:
    const float m_lsb;
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_LSBWIDGET_HPP */

