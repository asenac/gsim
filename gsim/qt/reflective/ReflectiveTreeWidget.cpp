// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * ReflectiveTreeWidget.cpp
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

#include "ReflectiveTreeWidget.hpp"
#include <gsim/qt/reflective/qvariant.hpp>
#include <gsim/qt/reflective/ReflectiveItemDelegate.hpp>
#include <gsim/core/properties.hpp>
#include <gsim/core/descriptor_utils.hpp>
#include <cassert>

using namespace gsim::qt;

QWidget * ReflectiveTreeWidget::create(TypeDescriptor_ptr reflective,
        QWidget * parent)
{
    return new ReflectiveTreeWidget(reflective, parent);
}

ReflectiveTreeWidget::ReflectiveTreeWidget(
        TypeDescriptor_ptr descriptor,
        QWidget * parent) :
    QTreeView(parent), 
    ReflectiveWidget(descriptor), 

    m_holder( ::gsim::core::utils::create_initialized_holder(descriptor)), 
    m_model(descriptor, m_holder, this)
{
    m_model.setEditable(true);

    setModel(&m_model);

    setItemDelegate(new ReflectiveItemDelegate(this));

    setBigWidget(true);

    setMinimumHeight(200);
    setColumnWidth(0, 300);
    setEditTriggers(QAbstractItemView::AllEditTriggers);

    connect(&m_model, 
            SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
            this,
            SIGNAL(editingFinished()));

    connect(&m_model, 
            SIGNAL(modelReset()),
            this,
            SIGNAL(editingFinished()));
}

ReflectiveTreeWidget::~ReflectiveTreeWidget()
{
}

void ReflectiveTreeWidget::toHolder(Holder& holder) 
{
    m_reflective->copy(m_holder, holder);
}

void ReflectiveTreeWidget::fromHolder(Holder& holder)
{
    m_reflective->copy(holder, m_holder);

    m_model.resetModel();
}

void ReflectiveTreeWidget::setValue(const QVariant& var)
{
    Holder h = m_reflective->create_holder();

    if (fromQVariant(m_reflective, h, var))
    {
        fromHolder(h);
    }
}

QVariant ReflectiveTreeWidget::value() 
{
    Holder h = m_reflective->create_holder();
    toHolder(h);
    return toQVariant(m_reflective, h);
}

void ReflectiveTreeWidget::save(QVariant& settings)
{
    settings = value();
}

void ReflectiveTreeWidget::load(const QVariant& settings)
{
    setValue(settings);
}

void ReflectiveTreeWidget::_setReadOnly(bool readOnly)
{
    if (readOnly)
        setEditTriggers(QAbstractItemView::NoEditTriggers);
    else
        setEditTriggers(QAbstractItemView::AllEditTriggers);
}

