// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * StructWidget.cpp
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

#include "StructWidget.hpp"
#include <gsim/qt/reflective/qvariant.hpp>

using namespace gsim::qt;

StructWidget::StructWidget(
        TypeDescriptor_ptr descriptor,
        WidgetFactory_t factory,
        QWidget * parent) :
    qt::FormWidget(parent), ReflectiveWidget(descriptor)
{
    setBigWidget(true);
    setSupportDrag(true);

    unsigned int count = descriptor->get_children_count();

    m_widgets.resize(count, NULL);

    for (unsigned int i = 0; i < count; i++) 
    {
        core::descriptor_ptr child = 
            descriptor->get_child(i);

        const char * child_name = descriptor->get_child_name(i);

        QWidget * child_widget = factory(child, this);

        m_widgets[i] = dynamic_cast< ReflectiveWidget* >(
                child_widget);

        bool isBig = false;

        // Se conecta la modificación de cada uno de los campos
        // a la modificación del formulario de la estructura
        if (m_widgets[i])
        {
            connect(child_widget, 
                    m_widgets[i]->editingFinishedSignal(),
                    this, SIGNAL(editingFinished()));

            isBig = m_widgets[i]->isBigWidget();
        }

        child_widget->setObjectName(child_name);

        if (isBig)
        {
            addBigField(child_name, child_widget);
        }
        else
        {
            addField(child_name, child_widget);
        }
    }
}

StructWidget::~StructWidget()
{
}

void StructWidget::toHolder(Holder& holder) 
{
    const unsigned int count = m_reflective->get_children_count();

    for (unsigned int i = 0; i < count; i++) 
    {
        if (m_widgets[i])
        {
            Holder child_holder(
                    m_reflective->get_child_value(holder, i));
            m_widgets[i]->toHolder(child_holder);
        }
    }
}

void StructWidget::fromHolder(Holder& holder)
{
    const unsigned int count = m_reflective->get_children_count();

    for (unsigned int i = 0; i < count; i++) 
    {
        if (m_widgets[i])
        {
            Holder child_holder(
                    m_reflective->get_child_value(holder, i));
            m_widgets[i]->fromHolder(child_holder);
        }
    }
}

void StructWidget::setValue(const QVariant& var)
{
    Holder h = m_reflective->create_holder();

    if (fromQVariant(m_reflective, h, var))
    {
        fromHolder(h);
    }
}

QVariant StructWidget::value() 
{
    Holder h = m_reflective->create_holder();
    toHolder(h);
    return toQVariant(m_reflective, h);
}

void StructWidget::save(QVariant& settings)
{
    unsigned int count = m_reflective->get_children_count();

    QVariantMap value;
    for (unsigned int i = 0; i < count; i++) 
    {
        if (m_widgets[i])
        {
            QVariant child;

            m_widgets[i]->save(child);

            value[m_reflective->get_child_name(i)] = child;
        }
    }

    settings = value;
}

void StructWidget::load(const QVariant& settings)
{
    const QVariantMap value = settings.toMap();

    unsigned int count = m_reflective->get_children_count();

    for (unsigned int i = 0; i < count; i++) 
    {
        if (m_widgets[i])
        {
            m_widgets[i]->load(
                    value[m_reflective->get_child_name(i)]);
        }
    }
}

void StructWidget::_setReadOnly(bool readOnly)
{
    unsigned int count = m_reflective->get_children_count();

    for (unsigned int i = 0; i < count; i++) 
    {
        if (m_widgets[i])
        {
            m_widgets[i]->_setReadOnly(readOnly);
        }
    }
}

