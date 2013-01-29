// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * FixedPointWidget.cpp
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

#include "FixedPointWidget.hpp"
#include <limits>
#include <cassert>
#include <cmath>

using namespace gsim::qt;

// FixedPoint widget

QWidget * FixedPointWidget::create(TypeDescriptor_ptr reflective,
        unsigned int fractionalBits,
        QWidget * parent)
{
    return new FixedPointWidget(reflective, fractionalBits, parent);
}

FixedPointWidget::FixedPointWidget(TypeDescriptor_ptr reflective,
        unsigned int fractionalBits,
        QWidget * parent) :
    FloatWidget(reflective, parent)
{
    setFractionalBits(fractionalBits);
}

FixedPointWidget::~FixedPointWidget()
{
}

unsigned int FixedPointWidget::fractionalBits() const
{
    return m_fractionalBits;
}

void FixedPointWidget::setFractionalBits(unsigned int bits)
{
    m_fractionalBits = bits;
    m_factor = (1 << bits);

    using namespace gsim::core;

    assert(m_fractionalBits > 0);
    assert(m_factor > 0);

    // Establece el número de decimales del numero de fractionalBits
    setDecimals((int) ((logf(m_factor - 1)/logf(10.0)) + 0.5));

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

    min = min / m_factor;
    max = max / m_factor;
    setRange(min, max);
}

void FixedPointWidget::toHolder(Holder& holder) 
{
    using namespace gsim::core;

    const descriptor_type type = m_reflective->get_type();

    double value_ = value() * m_factor;

    switch(type)
    {
        case TYPE_OCTET:
            holder.to_value< unsigned char >() = (unsigned char) value_;
            break;
        case TYPE_CHAR:
            holder.to_value< char >() = (char) value_;
            break;
        case TYPE_SHORT:
            holder.to_value< short >() = (short) value_;
            break;
        case TYPE_USHORT:
            holder.to_value< unsigned short >() = (unsigned short) value_;
            break;
        case TYPE_LONG:
            holder.to_value< int32_t >() = (int32_t) value_;
            break;
        case TYPE_ULONG:
            holder.to_value< uint32_t >() = (uint32_t) value_;
            break;
        case TYPE_LONGLONG:
            holder.to_value< int64_t >() = (int64_t) value_;
            break;
        case TYPE_ULONGLONG:
            holder.to_value< uint64_t >() = (uint64_t) value_;
            break;
        default:
            break;
    }
}

void FixedPointWidget::fromHolder(Holder& holder)
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
    val = val / m_factor;
    setValue(val);
}

//
// Helper
//

FixedPointHelper::FixedPointHelper(unsigned int fractionalBits) :
    m_fractionalBits(fractionalBits),
    m_factor(1 << fractionalBits)
{
}

FixedPointHelper::~FixedPointHelper()
{
}

QVariant FixedPointHelper::toQVariant(
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
    val = val / m_factor;

    return QVariant(val);
}

bool FixedPointHelper::fromQVariant(
        TypeDescriptor_ptr descriptor,
        Holder& holder,
        const QVariant& var) const
{
    using namespace gsim::core;

    if (!var.canConvert< double >())
        return false;

    const descriptor_type type = descriptor->get_type();

    double value_ = var.toDouble() * m_factor;

    switch(type)
    {
        case TYPE_OCTET:
            holder.to_value< unsigned char >() = (unsigned char) value_;
            break;
        case TYPE_CHAR:
            holder.to_value< char >() = (char) value_;
            break;
        case TYPE_SHORT:
            holder.to_value< short >() = (short) value_;
            break;
        case TYPE_USHORT:
            holder.to_value< unsigned short >() = (unsigned short) value_;
            break;
        case TYPE_LONG:
            holder.to_value< int32_t >() = (int32_t) value_;
            break;
        case TYPE_ULONG:
            holder.to_value< uint32_t >() = (uint32_t) value_;
            break;
        case TYPE_LONGLONG:
            holder.to_value< int64_t >() = (int64_t) value_;
            break;
        case TYPE_ULONGLONG:
            holder.to_value< uint64_t >() = (uint64_t) value_;
            break;
        default:
            break;
    }

    return true;
}

QVariantHelper_ptr FixedPointHelper::create(unsigned int fractionalBits)
{
    return QVariantHelper_ptr(new FixedPointHelper(fractionalBits));
}

