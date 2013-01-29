// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * BCDWidget.cpp
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

#include "BCDWidget.hpp"
#include <cmath>

using namespace gsim::qt;

template< typename T >
T to_bcd(T t)
{
    T res = 0;

    for (unsigned int i = 0; i < sizeof(T); i++) 
    {
        const T x = t % 100;
        T y = (x / 10) << 4;
        y = y | (x % 10);

        res |= (y << (i * 8));

        t /= 100;
    }

    return res;
}

template< typename T >
T from_bcd(T t)
{
    T res = 0;

    int k = 1;
    for (unsigned int i = 0; i < sizeof(T); i++) 
    {
        const T x = ((t & 0xF0) >> 4) * 10 + (t & 0xF);

        res += (x * k); 

        k *= 100;
        t = t >> 8;
    }

    return res;
}

template< typename T >
int max_bcd()
{
    return (int) std::pow((float) 100.0, (float) sizeof(T)) - 1;
}


// BCD widget

QWidget * BCDWidget::create(TypeDescriptor_ptr reflective,
        QWidget * parent)
{
    return new BCDWidget(reflective, parent);
}

BCDWidget::BCDWidget(TypeDescriptor_ptr reflective,
        QWidget * parent) :
    IntegerWidget(reflective, parent)
{
    using namespace gsim::core;

    const descriptor_type type = reflective->get_type();

    switch(type)
    {
        case TYPE_OCTET:
            setRange(0, max_bcd< unsigned char >());
            break;
        case TYPE_USHORT:
            setRange(0, max_bcd< unsigned short >());
            break;
        case TYPE_ULONG:
            setRange(0, max_bcd< uint32_t >());
            break;
        case TYPE_ULONGLONG:
            setRange(0, max_bcd< uint64_t >());
            break;
        default:
            break;
    }
}

BCDWidget::~BCDWidget()
{
}

void BCDWidget::toHolder(Holder& holder) 
{
    using namespace gsim::core;

    const descriptor_type type = m_reflective->get_type();

    int value_ = value();

    switch(type)
    {
        case TYPE_OCTET:
            holder.to_value< unsigned char >() = to_bcd((unsigned char) value_);
            break;
        case TYPE_USHORT:
            holder.to_value< unsigned short >() = to_bcd((unsigned short) value_);
            break;
        case TYPE_ULONG:
            holder.to_value< uint32_t >() = to_bcd((uint32_t) value_);
            break;
        case TYPE_ULONGLONG:
            holder.to_value< uint64_t >() = to_bcd((uint64_t) value_);
            break;
        default:
            break;
    }
}

void BCDWidget::fromHolder(Holder& holder)
{
    using namespace gsim::core;

    const descriptor_type type = m_reflective->get_type();

    switch(type)
    {
        case TYPE_OCTET:
            setValue(from_bcd(holder.to_value< unsigned char >()));
            break;
        case TYPE_USHORT:
            setValue(from_bcd(holder.to_value< unsigned short >()));
            break;
        case TYPE_ULONG:
            setValue(from_bcd(holder.to_value< uint32_t >()));
            break;
        case TYPE_ULONGLONG:
            setValue(from_bcd(holder.to_value< uint64_t >()));
            break;
        default:
            break;
    }
}

