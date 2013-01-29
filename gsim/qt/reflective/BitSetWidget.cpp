// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * BitSetWidget.cpp
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

#include "BitSetWidget.hpp"
#include <limits>
#include <cassert>
#include <cmath>

using namespace gsim::qt;

// BitSet widget

QWidget * BitSetWidget::create(TypeDescriptor_ptr reflective,
        QWidget * parent)
{
    return new BitSetWidget(reflective, QString(), parent);
}

QWidget * BitSetWidget::createWithTags(TypeDescriptor_ptr reflective,
        const QString& tags,
        QWidget * parent)
{
    return new BitSetWidget(reflective, tags, parent);
}

BitSetWidget::BitSetWidget(TypeDescriptor_ptr reflective,
        const QString& tags,
        QWidget * parent) :
    FormWidget(parent), ReflectiveWidget(reflective)
{
    const QStringList tagList (tags.split(';'));

    setBigWidget(true);

    using namespace gsim::core;

    const descriptor_type type = reflective->get_type();

    int bits = 0;

    switch(type)
    {
        case TYPE_OCTET:
        case TYPE_CHAR:
            bits = 8;
            break;
        case TYPE_SHORT:
        case TYPE_USHORT:
            bits = 16;
            break;
        case TYPE_LONG:
        case TYPE_ULONG:
            bits = 32;
            break;
        case TYPE_LONGLONG:
        case TYPE_ULONGLONG:
            bits = 64;
            break;
        default:
            break;
    }

    for (int i = 0; i < bits; i++) 
    {
        const QString tag = ((i < tagList.size() && !tagList.at(i).isEmpty())? 
                tagList.at(i): 
                QString::number(i));

        QCheckBox * sp = new QCheckBox();
        addField(tag, sp);
        m_spinBoxes.push_back(sp);

        connect(sp, SIGNAL(stateChanged(int)), this,
                SIGNAL(editingFinished()));
    }
}

BitSetWidget::~BitSetWidget()
{
}

namespace  
{

template < typename T >
T toValue(const std::vector< QCheckBox * >& sps)
{
    T res = 0;

    for (int i = sps.size() - 1; i >= 0; i--) 
    {
        T val = ((sps[i]->isChecked())? 1: 0);

        res |= (val << i);
    }

    return res;
}

template < typename T >
void fromValue(std::vector< QCheckBox * >& sps, Holder& holder)
{
    T val = holder.to_value< T >();

    for (int i = sps.size() - 1; i >= 0; i--) 
    {
        T mask = (1 << i);

        sps[i]->setChecked((val & mask) != 0);
    }
}

} // namespace

void BitSetWidget::toHolder(Holder& holder) 
{
    using namespace gsim::core;

    const descriptor_type type = m_reflective->get_type();

    switch(type)
    {
        case TYPE_OCTET:
            holder.to_value< unsigned char >() = toValue< unsigned char >(m_spinBoxes);
            break;
        case TYPE_CHAR:
            holder.to_value< char >() = toValue< char >(m_spinBoxes);
            break;
        case TYPE_SHORT:
            holder.to_value< short >() =  toValue< short >(m_spinBoxes);
            break;
        case TYPE_USHORT:
            holder.to_value< unsigned short >() = toValue< unsigned short >(m_spinBoxes);
            break;
        case TYPE_LONG:
            holder.to_value< int32_t >() = toValue< int32_t >(m_spinBoxes);
            break;
        case TYPE_ULONG:
            holder.to_value< uint32_t >() = toValue< uint32_t >(m_spinBoxes);
            break;
        case TYPE_LONGLONG:
            holder.to_value< int64_t >() = toValue< int64_t >(m_spinBoxes);
            break;
        case TYPE_ULONGLONG:
            holder.to_value< uint64_t >() = toValue< uint64_t >(m_spinBoxes);
            break;
        default:
            break;
    }
}

void BitSetWidget::fromHolder(Holder& holder)
{
    using namespace gsim::core;

    const descriptor_type type = m_reflective->get_type();

    switch(type)
    {
        case TYPE_OCTET:
            fromValue< unsigned char >(m_spinBoxes, holder);
            break;
        case TYPE_CHAR:
            fromValue< char >(m_spinBoxes, holder);
            break;
        case TYPE_SHORT:
            fromValue< short >(m_spinBoxes, holder);
            break;
        case TYPE_USHORT:
            fromValue< unsigned short >(m_spinBoxes, holder);
            break;
        case TYPE_LONG:
            fromValue< int32_t >(m_spinBoxes, holder);
            break;
        case TYPE_ULONG:
            fromValue< uint32_t >(m_spinBoxes, holder);
            break;
        case TYPE_LONGLONG:
            fromValue< int64_t >(m_spinBoxes, holder);
            break;
        case TYPE_ULONGLONG:
            fromValue< uint64_t >(m_spinBoxes, holder);
            break;
        default:
            break;
    }
}

void BitSetWidget::save(QVariant& settings)
{
    QVariantList list;

    for (unsigned int i = 0; i < m_spinBoxes.size(); i++) 
    {
        list << m_spinBoxes[i]->isChecked();
    }

    settings = list;
}

void BitSetWidget::load(const QVariant& settings)
{
    const QVariantList list = settings.toList();

    for (int i = 0; i < (int) m_spinBoxes.size() 
            && i < list.size(); i++) 
    {
        m_spinBoxes[i]->setChecked(list.at(i).toBool());
    }
}

void BitSetWidget::_setReadOnly(bool readOnly)
{
    for (unsigned int i = 0; i < m_spinBoxes.size(); i++) 
    {
        m_spinBoxes[i]->setEnabled(!readOnly);
    }
}

