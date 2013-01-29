// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * FixedPointWidget.hpp
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

#ifndef GSIM_QT_FIXEDPOINTWIDGET_HPP
#define GSIM_QT_FIXEDPOINTWIDGET_HPP

#include <QtGui>

#include <gsim/qt/export.hpp>
#include <gsim/qt/reflective/FloatWidget.hpp>
#include <gsim/qt/reflective/QVariantHelper.hpp>

namespace gsim 
{
namespace qt 
{

class GSIM_QT_DECLSPEC FixedPointWidget : 
    public FloatWidget
{
    Q_OBJECT
    Q_PROPERTY(unsigned int fractionalBits READ fractionalBits
	WRITE setFractionalBits)
public:
    FixedPointWidget(TypeDescriptor_ptr reflective,
            unsigned int fractionalBits,
            QWidget * parent = 0);
    virtual ~FixedPointWidget();

    virtual void toHolder(Holder& holder);
    virtual void fromHolder(Holder& holder);

    static QWidget * create(TypeDescriptor_ptr reflective,
            unsigned int fractionalBits,
            QWidget * parent = 0);

    unsigned int fractionalBits() const;

public slots:

    void setFractionalBits(unsigned int bits);

protected:

    unsigned int m_fractionalBits;
    unsigned int m_factor;
};

class FixedPointHelper : public QVariantHelper
{
public:
    FixedPointHelper(unsigned int fractionalBits);
    ~FixedPointHelper();

    QVariant toQVariant(
            TypeDescriptor_ptr descriptor,
            Holder& hold) const;

    bool fromQVariant(
            TypeDescriptor_ptr descriptor,
            Holder& hold,
            const QVariant& var) const;

    static QVariantHelper_ptr create(unsigned int fractionalBits);

protected:
    const unsigned int m_fractionalBits;
    const unsigned int m_factor;
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_FIXEDPOINTWIDGET_HPP */

