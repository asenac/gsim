// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * TableWidget.cpp
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

#include "TableWidget.hpp"
#include <gsim/qt/reflective/qvariant.hpp>
#include <gsim/core/properties.hpp>
#include <gsim/qt/reflective/WidgetFactory.hpp>
#include <cassert>

using namespace gsim::qt;

QWidget * TableWidget::create(TypeDescriptor_ptr reflective,
        QWidget * parent)
{
    return new TableWidget(reflective, parent);
}

TableWidget::TableWidget(
        TypeDescriptor_ptr descriptor,
        QWidget * parent) :
    QTableWidget(parent), ReflectiveWidget(descriptor)
{
    const core::descriptor_type type_ = descriptor->get_type();

    assert(type_ == core::TYPE_STRUCT || type_ == core::TYPE_ARRAY);

    setBigWidget(true);

    // El scroll vertical siempre está desactivado.
    // Ha de estar contenido en un scroll vertical.
    // TODO ver como ajustar el tamaño para que se muestren
    // todas las filas.
    // setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    if (type_ == core::TYPE_STRUCT)
    {
        QStringList headers;

        unsigned int count = descriptor->get_children_count();

        m_widgets.resize(1);
        m_widgets[0].resize(count, NULL);

        // Establece las dimensiones
        setRowCount(1);
        setColumnCount(count);

        for (unsigned int i = 0; i < count; i++) 
        {
            core::descriptor_ptr child = 
                descriptor->get_child(i);

            const char * child_name = descriptor->get_child_name(i);
            headers << child_name;

            QWidget * child_widget = WidgetFactoryRepository::create(child, this);

            m_widgets[0][i] = dynamic_cast< ReflectiveWidget* >(child_widget);

            setCellWidget(0, i, child_widget);
        }

        setHorizontalHeaderLabels(headers);
        setVerticalHeaderLabels(QStringList() << "");
    }
    else if (type_ == core::TYPE_ARRAY)
    {
        TypeDescriptor_ptr slice = descriptor->get_slice();
        const core::descriptor_type sliceType = slice->get_type();

        unsigned int count = descriptor->get_children_count();
        m_widgets.resize(count);

        // Establece las dimensiones
        setRowCount(count);

        if (sliceType == core::TYPE_STRUCT || sliceType == core::TYPE_ARRAY)
        {
            unsigned int sliceCount = slice->get_children_count();
            QStringList headers;

            // Establece las dimensiones
            setColumnCount(sliceCount);

            for (unsigned int i = 0; i < count; i++) 
            {
                m_widgets[i].resize(sliceCount);

                for (unsigned int j = 0; j < sliceCount; j++) 
                {
                    core::descriptor_ptr child = slice->get_child(j);

                    // Solo se rellena en la primera pasada
                    if (!i)
                    {
                        if (sliceType == core::TYPE_STRUCT)
                            headers << slice->get_child_name(j); 
                        else
                            headers << QString::number(j);
                    }

                    QWidget * child_widget = WidgetFactoryRepository::create(child, this);

                    m_widgets[i][j] = dynamic_cast< ReflectiveWidget* >(child_widget);

                    setCellWidget(i, j, child_widget);
                }
            }

            setHorizontalHeaderLabels(headers);
        }
        else
        {
            // TODO
        }
    }

    // Ajusta el tamaño de las columnas
    for (int i = 0; i < columnCount(); i++) 
    {
        horizontalHeader()->setResizeMode(i, QHeaderView::ResizeToContents);
    }
}

TableWidget::~TableWidget()
{
}

void TableWidget::rowToStructHolder(int idx, Holder& holder)
{
    TypeDescriptor_ptr descriptor = holder.get_type_descriptor();
    unsigned int count = descriptor->get_children_count();

    for (unsigned int i = 0; i < count; i++) 
    {
        Holder ch(descriptor->get_child_value(holder, i));
        m_widgets[idx][i]->toHolder(ch);
    }
}

void TableWidget::rowFromStructHolder(int idx, Holder& holder)
{
    TypeDescriptor_ptr descriptor = holder.get_type_descriptor();
    unsigned int count = descriptor->get_children_count();

    for (unsigned int i = 0; i < count; i++) 
    {
        Holder ch(descriptor->get_child_value(holder, i));
        m_widgets[idx][i]->fromHolder(ch);
    }
}

void TableWidget::toHolder(Holder& holder) 
{
    const core::descriptor_type type_ = m_reflective->get_type();

    if (type_ == core::TYPE_STRUCT)
    {
        rowToStructHolder(0, holder);
    }
    else if (type_ == core::TYPE_ARRAY)
    {
        TypeDescriptor_ptr slice = m_reflective->get_slice();
        const core::descriptor_type sliceType = slice->get_type();

        if (sliceType == core::TYPE_STRUCT || core::TYPE_ARRAY)
        {
            unsigned int count = m_reflective->get_children_count();

            for (unsigned int i = 0; i < count; i++) 
            {
                Holder ch(m_reflective->get_child_value(holder, i));
                rowToStructHolder(i, ch);
            }
        }
        else
        {
            // TODO
        }
    }
}

void TableWidget::fromHolder(Holder& holder)
{
    const core::descriptor_type type_ = m_reflective->get_type();

    if (type_ == core::TYPE_STRUCT)
    {
        rowFromStructHolder(0, holder);
    }
    else if (type_ == core::TYPE_ARRAY)
    {
        TypeDescriptor_ptr slice = m_reflective->get_slice();
        const core::descriptor_type sliceType = slice->get_type();

        if (sliceType == core::TYPE_STRUCT || core::TYPE_ARRAY)
        {
            unsigned int count = m_reflective->get_children_count();

            for (unsigned int i = 0; i < count; i++) 
            {
                Holder ch(m_reflective->get_child_value(holder, i));
                rowFromStructHolder(i, ch);
            }
        }
        else
        {
            // TODO
        }
    }
}

void TableWidget::setValue(const QVariant& var)
{
    Holder h = m_reflective->create_holder();

    if (fromQVariant(m_reflective, h, var))
    {
        fromHolder(h);
    }
}

QVariant TableWidget::value() 
{
    Holder h = m_reflective->create_holder();
    toHolder(h);
    return toQVariant(m_reflective, h);
}

void TableWidget::save(QVariant& settings)
{
    settings = value();
}

void TableWidget::load(const QVariant& settings)
{
    setValue(settings);
}

void TableWidget::_setReadOnly(bool readOnly)
{
    for (unsigned int i = 0; i < m_widgets.size(); i++) 
    {
        for (unsigned int j = 0; j < m_widgets[i].size(); j++) 
        {
            if (m_widgets[i][j])
            {
                m_widgets[i][j]->_setReadOnly(readOnly);
            }
        }
    }
}

