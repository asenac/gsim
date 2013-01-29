// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * IPAddressWidget.cpp
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

#include "IPAddressWidget.hpp"
#include <cassert>
#include <QHostAddress>

using namespace gsim::qt;

// IPAddress widget

QWidget * IPAddressWidget::create(TypeDescriptor_ptr reflective,
        QWidget * parent)
{
    return new IPAddressWidget(reflective, parent);
}

IPAddressWidget::IPAddressWidget(TypeDescriptor_ptr reflective,
        QWidget * parent) :
    IpAddressLineEdit(parent), ReflectiveWidget(reflective)
{
    using namespace gsim::core;

    Holder dummy;

    assert(
            // 4 octetos
            reflective->get_type() == TYPE_ULONG 

            // Array de 4 octetos
            || (reflective->get_type() == TYPE_ARRAY && 
                reflective->get_length(dummy) == 4 &&
                reflective->get_slice()->get_type() == TYPE_OCTET)

            // Estructura con 4 octetos
            || (reflective->get_type() == TYPE_STRUCT &&
                reflective->get_children_count() == 4 &&
                reflective->get_child(0)->get_type() == TYPE_OCTET &&
                reflective->get_child(1)->get_type() == TYPE_OCTET &&
                reflective->get_child(2)->get_type() == TYPE_OCTET &&
                reflective->get_child(3)->get_type() == TYPE_OCTET)

            // String
            || (reflective->get_type() == TYPE_STRING));
}

IPAddressWidget::~IPAddressWidget()
{
}

void IPAddressWidget::toHolder(Holder& holder) 
{
    using namespace gsim::core;

    const descriptor_type type = m_reflective->get_type();

    const QHostAddress address(text());

    switch(type)
    {
        case TYPE_STRUCT:
        case TYPE_ARRAY:
            {
                const uint32_t val = address.toIPv4Address();

                for (int i = 0; i < 4; i++) 
                {
                    Holder ch (m_reflective->get_child_value(holder, i));

                    ch.to_value< unsigned char >() = 
                        (unsigned char) ((val >> (24 - i * 8)) & 0xFF);
                }
            }
            break;
        case TYPE_ULONG:
            holder.to_value< uint32_t >() = (uint32_t) address.toIPv4Address();
            break;
        case TYPE_STRING:
            m_reflective->from_string(holder, address.toString().toStdString());
            break;
        default:
            break;
    }
}

void IPAddressWidget::fromHolder(Holder& holder)
{
    using namespace gsim::core;

    const descriptor_type type = m_reflective->get_type();

    QHostAddress address;

    switch(type)
    {
        case TYPE_ULONG:
            address.setAddress(holder.to_value< uint32_t >());
            break;
        case TYPE_STRING:
            {
                const std::string str = m_reflective->to_string(holder);
                address.setAddress(str.c_str());
            }
            break;
        case TYPE_STRUCT:
        case TYPE_ARRAY:
            {
                uint32_t val = 0;

                for (int i = 0; i < 4; i++) 
                {
                    Holder ch (m_reflective->get_child_value(holder, i));

                    uint32_t b = (uint32_t) ch.to_value< unsigned char >();

                    val |= (b << (24 - i * 8));
                }

                address.setAddress(val);
            }
            break;
        default:
            break;
    }

    setText(address.toString());
}

void IPAddressWidget::save(QVariant& settings)
{
    settings = text();
}

void IPAddressWidget::load(const QVariant& settings)
{
    setText(settings.toString());
}

void IPAddressWidget::_setReadOnly(bool readOnly)
{
    setReadOnly(readOnly);
}

