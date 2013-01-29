// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * HexWidget.cpp
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

#include "HexWidget.hpp"

using namespace gsim::qt;

// Hex widget

QWidget * HexWidget::create(TypeDescriptor_ptr reflective,
        QWidget * parent)
{
    return new HexWidget(reflective, parent);
}

HexWidget::HexWidget(TypeDescriptor_ptr reflective,
        QWidget * parent) :
    QLineEdit(parent), ReflectiveWidget(reflective)
{
    using namespace gsim::core;

    const descriptor_type type = reflective->get_type();

    int size = 0;

    switch(type)
    {
        case TYPE_OCTET:
            size = sizeof(unsigned char);
            break;
        case TYPE_USHORT:
            size = sizeof(unsigned short);
            break;
        case TYPE_ULONG:
            size = sizeof(uint32_t);
            break;
        case TYPE_ULONGLONG:
            size = sizeof(uint64_t);
            break;
        default:
            break;
    }

    QString value;
    QString mask;
    for (int i = 0; i < size; i++) 
    {
        mask += "hh";
        value += "00";
    }

    setInputMask(mask);
    setText(value);
}

HexWidget::~HexWidget()
{
}

void HexWidget::toHolder(Holder& holder) 
{
    using namespace gsim::core;

    const descriptor_type type = m_reflective->get_type();

    bool ok = false;
    const QString value_ = text();

    switch(type)
    {
        case TYPE_OCTET:
            holder.to_value< unsigned char >() = 
                (unsigned char) value_.toUShort(&ok, 16);
            break;
        case TYPE_USHORT:
            holder.to_value< unsigned short >() = value_.toUShort(&ok, 16);
            break;
        case TYPE_ULONG:
            holder.to_value< uint32_t >() = value_.toULong(&ok, 16);
            break;
        case TYPE_ULONGLONG:
            holder.to_value< uint64_t >() = value_.toULongLong(&ok, 16);
            break;
        default:
            break;
    }
}

void HexWidget::fromHolder(Holder& holder)
{
    using namespace gsim::core;

    const descriptor_type type = m_reflective->get_type();

    switch(type)
    {
        case TYPE_OCTET:
            setText(QString::number(holder.to_value< unsigned char >(), 16));
            break;
        case TYPE_USHORT:
            setText(QString::number(holder.to_value< unsigned short >(), 16));
            break;
        case TYPE_ULONG:
            setText(QString::number(holder.to_value< uint32_t >(), 16));
            break;
        case TYPE_ULONGLONG:
            setText(QString::number(holder.to_value< uint64_t >(), 16));
            break;
        default:
            break;
    }
}

void HexWidget::save(QVariant& settings)
{
    settings = text();
}

void HexWidget::load(const QVariant& settings)
{
    setText(settings.toString());
}

void HexWidget::_setReadOnly(bool readOnly)
{
    setReadOnly(readOnly);
}

