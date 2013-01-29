// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * FloatWidget.cpp
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

#include "FloatWidget.hpp"
#include <limits>

using namespace gsim::qt;

class FloatWidget::Data
{
public:

    double validMinimum;
    double validMaximum;
};

// Float widget

FloatWidget::FloatWidget(TypeDescriptor_ptr reflective,
        QWidget * parent) :
    QDoubleSpinBox(parent), ReflectiveWidget(reflective), 
    m_data(new Data)
{
    m_data->validMinimum = -std::numeric_limits< double >::max();
    m_data->validMaximum = std::numeric_limits< double >::max();

    using namespace gsim::core;

    setDecimals(10);

    const descriptor_type type = reflective->get_type();

    switch(type)
    {
        case TYPE_DOUBLE:
            {
                double max = std::numeric_limits< double >::max(),
                       min = -std::numeric_limits< double >::max();

                setRange(min, max);
            }
            break;
        case TYPE_FLOAT:
            setRange(-std::numeric_limits< float >::max(),
                    std::numeric_limits< float >::max());
            break;
        default:
            break;
    }

    connect(this, SIGNAL(valueChanged(double)), this,
                SLOT(checkValidValue(double)));

    // Actualiza el color de fondo actual
    checkValidValue(value());
}

FloatWidget::~FloatWidget()
{
    delete m_data;
}

void FloatWidget::toHolder(Holder& holder) 
{
    using namespace gsim::core;

    const descriptor_type type = m_reflective->get_type();

    double value_ = value();

    switch(type)
    {
        case TYPE_DOUBLE:
            holder.to_value< double >() = (double) value_;
            break;
        case TYPE_FLOAT:
            holder.to_value< float >() = (float) value_;
            break;
        default:
            break;
    }
}

void FloatWidget::fromHolder(Holder& holder)
{
    using namespace gsim::core;

    const descriptor_type type = m_reflective->get_type();

    switch(type)
    {
        case TYPE_DOUBLE:
            setValue(holder.to_value< double >());
            break;
        case TYPE_FLOAT:
            setValue(holder.to_value< float >());
            break;
        default:
            break;
    }
}

void FloatWidget::save(QVariant& settings)
{
    settings = value();
}

void FloatWidget::load(const QVariant& settings)
{
    if (settings.canConvert< double >())
    {
        setValue(settings.toDouble());
    }
}

void FloatWidget::_setReadOnly(bool readOnly)
{
    setReadOnly(readOnly);
}

void FloatWidget::checkValidValue(double value)
{
    bool valid = (value >= m_data->validMinimum) &&
        (value <= m_data->validMaximum);

    if (valid)
    {
       setStyleSheet("QDoubleSpinBox { background-color: white; }"); 
    }
    else
    {
       setStyleSheet("QDoubleSpinBox { background-color: red; }"); 
    }
}

double FloatWidget::validMinimum() const
{
    return m_data->validMinimum;
}

double FloatWidget::validMaximum() const
{
    return m_data->validMaximum;
}

void FloatWidget::setValidMinimum(double m)
{
    m_data->validMinimum = m;

    checkValidValue(value());
}

void FloatWidget::setValidMaximum(double m)
{
    m_data->validMaximum = m;

    checkValidValue(value());
}
