// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * ReflectiveGUI.cpp
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

#include "ReflectiveWidget.hpp"
#include <gsim/qt/reflective/WidgetFactory.hpp>
#include <gsim/core/types_repository.hpp>

using namespace gsim::qt;

// Widget base

ReflectiveWidget::ReflectiveWidget(
        TypeDescriptor_ptr reflective) :
    m_reflective(reflective), 
    m_isBigWidget(false), 
    m_supportDrag(false)
{
}

ReflectiveWidget::~ReflectiveWidget()
{
}

TypeDescriptor_ptr 
ReflectiveWidget::getReflective() const
{
    return m_reflective;
}

void ReflectiveWidget::_setReadOnly(bool readOnly)
{
}

const char * ReflectiveWidget::editingFinishedSignal() const
{
    return SIGNAL(editingFinished());
}

bool ReflectiveWidget::isBigWidget() const
{
    return m_isBigWidget;
}

void ReflectiveWidget::setBigWidget(bool isBig)
{
    m_isBigWidget = isBig;
}

bool ReflectiveWidget::supportDrag() const
{
    return m_supportDrag;
}

void ReflectiveWidget::setSupportDrag(bool d)
{
    m_supportDrag = d;
}

//
// ReflectiveQWidget
//

ReflectiveQWidget::ReflectiveQWidget(const QString& fqn, 
        QWidget * parent) :
    QWidget(parent), m_impl(NULL)
{
    core::types_repository const * repository = 
        core::types_repository::instance();

    TypeDescriptor_ptr descriptor = 
        repository->get_descriptor(fqn.toStdString());

    QVBoxLayout * l = new QVBoxLayout();

    if (descriptor)
    {
        QWidget * w = WidgetFactoryRepository::create(descriptor, this);
        l->addWidget(w);
        l->setMargin(0);

        // Puede no ser un ReflectiveWidget en caso de ser un
        // tipo no soportado
        m_impl = dynamic_cast< ReflectiveWidget * >(w);

        if (m_impl)
        {
            connect(w, m_impl->editingFinishedSignal(),
                    this, SIGNAL(editingFinished()));
        }
    }
    else
    {
        l->addWidget(new QLabel(
                    QString("Unregistered type '%1'").arg(fqn)));
    }

    setLayout(l);
}

ReflectiveQWidget::~ReflectiveQWidget()
{
}

ReflectiveWidget * ReflectiveQWidget::implementation() const
{
    return m_impl;
}

