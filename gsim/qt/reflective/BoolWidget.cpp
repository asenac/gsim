// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * BoolWidget.cpp
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

#include "BoolWidget.hpp"

using namespace gsim::qt;

BoolWidget::BoolWidget(TypeDescriptor_ptr reflective,
        QWidget * parent) :
    QCheckBox(parent), ReflectiveWidget(reflective)
{
}

BoolWidget::~BoolWidget()
{
}

void BoolWidget::toHolder(Holder& holder) 
{
    using namespace gsim::core;

    const descriptor_type type = m_reflective->get_type();

    switch(type)
    {
        case TYPE_BOOL:
            holder.to_value< bool >() = isChecked();
            break;
        default:
            break;
    }
}

void BoolWidget::fromHolder(Holder& holder)
{
    using namespace gsim::core;

    const descriptor_type type = m_reflective->get_type();

    switch(type)
    {
        case TYPE_BOOL:
            setChecked(holder.to_value< bool >());
            break;
        default:
            break;
    }
}

void BoolWidget::save(QVariant& settings)
{
    settings = isChecked();
}

void BoolWidget::load(const QVariant& settings)
{
    if (settings.canConvert< bool >())
    {
        setChecked(settings.toBool());
    }
}

void BoolWidget::_setReadOnly(bool readOnly)
{
    setEnabled(!readOnly);
}

const char * BoolWidget::editingFinishedSignal() const
{
    return SIGNAL(stateChanged(int));
}

