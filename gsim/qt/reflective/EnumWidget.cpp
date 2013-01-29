// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * EnumWidget.cpp
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

#include "EnumWidget.hpp"

using namespace gsim::qt;

// Enum Widget

EnumWidget::EnumWidget(TypeDescriptor_ptr reflective,
        QWidget * parent) :
    QComboBox(parent), ReflectiveWidget(reflective)
{
    assert(reflective->is_enum());

    unsigned int count = reflective->get_children_count();

    for (unsigned int i = 0; i < count; i++) 
    {
        addItem(reflective->get_child_name(i));
    }
}

EnumWidget::~EnumWidget()
{
}

void EnumWidget::toHolder(Holder& holder) 
{
    Holder tmp(m_reflective->get_child_value(holder, 
                (unsigned int) currentIndex()));

    m_reflective->copy(tmp, holder);
}

void EnumWidget::fromHolder(Holder& holder)
{
    int idx = -1;

    const unsigned int count = 
        m_reflective->get_children_count();

    // Obtiene el indice para el valor indicado

    for (unsigned int i = 0; i < count; i++) 
    {
        Holder tmp(m_reflective->get_child_value(holder, i));

        if (holder.to_value< int32_t >() ==
                tmp.to_value< int32_t >())
        {
            idx = (int) i;
            break;
        }
    }

    if (idx > -1 && idx < (int) count)
    {
        setCurrentIndex(idx);
    }
}

void EnumWidget::save(QVariant& settings)
{
    settings = currentText();
}

void EnumWidget::load(const QVariant& settings)
{
    if (settings.canConvert< QString >())
    {
        const QString text(settings.toString());

        // find corresponding index to convert text
        
        int idx = findText(text);

        if (idx != -1) setCurrentIndex(idx);
    }
}

void EnumWidget::_setReadOnly(bool readOnly)
{
    setEnabled(!readOnly);
}

const char * EnumWidget::editingFinishedSignal() const
{
    return SIGNAL(currentIndexChanged(int));
}

