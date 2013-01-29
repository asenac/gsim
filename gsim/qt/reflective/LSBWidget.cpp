// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * LSBWidget.cpp
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

#include "LSBWidget.hpp"
#include <limits>
#include <cassert>
#include <cmath>
#include <boost/bind.hpp>

using namespace gsim::qt;

// LSB widget

QWidget * LSBWidget::create(TypeDescriptor_ptr reflective,
        float lsb,
        QWidget * parent)
{
    return new LSBWidget(reflective, lsb, parent);
}

LSBWidget::LSBWidget(TypeDescriptor_ptr reflective,
        float lsb,
        QWidget * parent) :
    FloatWidget(reflective, parent)
{
    setLSB(lsb);
}

LSBWidget::~LSBWidget()
{
}

float LSBWidget::LSB() const
{
    return m_lsb;
}

void LSBWidget::setLSB(float lsb)
{
    m_lsb = lsb;

    using namespace gsim::core;

    assert(lsb > 0.0);

    // Establece el número de decimales en función del LSB.
    setDecimals((int) (logf(1.0/lsb) / logf(10.0)));

    const descriptor_type type = m_reflective->get_type();

    double min = 0.0, max = 0.0;

    switch(type)
    {
        case TYPE_OCTET:
            min = (double) std::numeric_limits< unsigned char >::min();
            max = (double) std::numeric_limits< unsigned char >::max();
            break;
        case TYPE_CHAR:
            min = (double) std::numeric_limits< char >::min();
            max = (double) std::numeric_limits< char >::max();
            break;
        case TYPE_SHORT:
            min = (double) std::numeric_limits< unsigned short >::min();
            max = (double) std::numeric_limits< unsigned short >::max();
            break;
        case TYPE_USHORT:
            min = (double) std::numeric_limits< unsigned short >::min();
            max = (double) std::numeric_limits< unsigned short >::max();
            break;
        case TYPE_LONG:
            min = (double) std::numeric_limits< int32_t >::min();
            max = (double) std::numeric_limits< int32_t >::max();
            break;
        case TYPE_ULONG:
            min = (double) std::numeric_limits< uint32_t >::min();
            max = (double) std::numeric_limits< uint32_t >::max();
            break;
        case TYPE_LONGLONG:
            min = (double) std::numeric_limits< int64_t >::min();
            max = (double) std::numeric_limits< int64_t >::max();
            break;
        case TYPE_ULONGLONG:
            min = (double) std::numeric_limits< uint64_t >::min();
            max = (double) std::numeric_limits< uint64_t >::max();
            break;
        default:
            break;
    }

    min = min * lsb;
    max = max * lsb;
    setRange(min, max);
}

void LSBWidget::toHolder(Holder& holder) 
{
    using namespace gsim::core;

    const descriptor_type type = m_reflective->get_type();

    double value_ = value() / m_lsb;

    switch(type)
    {
        case TYPE_OCTET:
            holder.to_value< unsigned char >() = (unsigned char) ceil(value_);
            break;
        case TYPE_CHAR:
            holder.to_value< char >() = (char) ceil(value_);
            break;
        case TYPE_SHORT:
            holder.to_value< short >() = (short) ceil(value_);
            break;
        case TYPE_USHORT:
            holder.to_value< unsigned short >() = (unsigned short) ceil(value_);
            break;
        case TYPE_LONG:
            holder.to_value< int32_t >() = (int32_t) ceil(value_);
            break;
        case TYPE_ULONG:
            holder.to_value< uint32_t >() = (uint32_t) ceil(value_);
            break;
        case TYPE_LONGLONG:
            holder.to_value< int64_t >() = (int64_t) ceil(value_);
            break;
        case TYPE_ULONGLONG:
            holder.to_value< uint64_t >() = (uint64_t) ceil(value_);
            break;
        default:
            break;
    }
}

void LSBWidget::fromHolder(Holder& holder)
{
    using namespace gsim::core;

    const descriptor_type type = m_reflective->get_type();

    double val = 0.0;
    switch(type)
    {
        case TYPE_OCTET:
            val = (double) (holder.to_value< unsigned char >());
            break;
        case TYPE_CHAR:
            val = (double) (holder.to_value< char >());
            break;
        case TYPE_SHORT:
            val = (double) (holder.to_value< short >());
            break;
        case TYPE_USHORT:
            val = (double) (holder.to_value< unsigned short >());
            break;
        case TYPE_LONG:
            val = (double) (holder.to_value< int32_t >());
            break;
        case TYPE_ULONG:
            val = (double) (holder.to_value< uint32_t >());
            break;
        case TYPE_LONGLONG:
            val = (double) (holder.to_value< int64_t >());
            break;
        case TYPE_ULONGLONG:
            val = (double) (holder.to_value< uint64_t >());
            break;
        default:
            break;
    }
    val = val * m_lsb;
    setValue(val);
}

//
// Helper
//

LSBHelper::LSBHelper(float lsb) :
    m_lsb(lsb)
{
}

LSBHelper::~LSBHelper()
{
}

QVariant LSBHelper::toQVariant(
        TypeDescriptor_ptr descriptor,
        Holder& holder) const
{
    using namespace gsim::core;

    const descriptor_type type = descriptor->get_type();

    double val = 0.0;
    switch(type)
    {
        case TYPE_OCTET:
            val = (double) (holder.to_value< unsigned char >());
            break;
        case TYPE_CHAR:
            val = (double) (holder.to_value< char >());
            break;
        case TYPE_SHORT:
            val = (double) (holder.to_value< short >());
            break;
        case TYPE_USHORT:
            val = (double) (holder.to_value< unsigned short >());
            break;
        case TYPE_LONG:
            val = (double) (holder.to_value< int32_t >());
            break;
        case TYPE_ULONG:
            val = (double) (holder.to_value< uint32_t >());
            break;
        case TYPE_LONGLONG:
            val = (double) (holder.to_value< int64_t >());
            break;
        case TYPE_ULONGLONG:
            val = (double) (holder.to_value< uint64_t >());
            break;
        default:
            break;
    }
    val = val * m_lsb;

    return QVariant(val);
}

bool LSBHelper::fromQVariant(
        TypeDescriptor_ptr descriptor,
        Holder& holder,
        const QVariant& var) const
{
    using namespace gsim::core;

    if (!var.canConvert< double >())
        return false;

    const descriptor_type type = descriptor->get_type();

    double value_ = var.toDouble() / m_lsb;

    switch(type)
    {
        case TYPE_OCTET:
            holder.to_value< unsigned char >() = (unsigned char) ceil(value_);
            break;
        case TYPE_CHAR:
            holder.to_value< char >() = (char) ceil(value_);
            break;
        case TYPE_SHORT:
            holder.to_value< short >() = (short) ceil(value_);
            break;
        case TYPE_USHORT:
            holder.to_value< unsigned short >() = (unsigned short) ceil(value_);
            break;
        case TYPE_LONG:
            holder.to_value< int32_t >() = (int32_t) ceil(value_);
            break;
        case TYPE_ULONG:
            holder.to_value< uint32_t >() = (uint32_t) ceil(value_);
            break;
        case TYPE_LONGLONG:
            holder.to_value< int64_t >() = (int64_t) ceil(value_);
            break;
        case TYPE_ULONGLONG:
            holder.to_value< uint64_t >() = (uint64_t) ceil(value_);
            break;
        default:
            break;
    }

    return true;
}

QVariantHelper_ptr LSBHelper::create(float lsb)
{
    return QVariantHelper_ptr(new LSBHelper(lsb));
}


