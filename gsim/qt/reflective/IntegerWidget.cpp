// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * IntegerWidget.cpp
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

#include "IntegerWidget.hpp"
#include <limits>
#include <map>

using namespace gsim::qt;

// Integer widget

QWidget * IntegerWidget::create(TypeDescriptor_ptr reflective,
        QWidget * parent)
{
    return new IntegerWidget(reflective, parent);
}

QWidget * IntegerWidget::createWithEnum(
        TypeDescriptor_ptr reflective,
        TypeDescriptor_ptr enumDescriptor,
        QWidget * parent)
{
    return new IntegerWidget(reflective, parent, 
            QString(), enumDescriptor);
}

QWidget * IntegerWidget::createWithTags(
        TypeDescriptor_ptr reflective,
        const QString& tags,
        QWidget * parent)
{
    return new IntegerWidget(reflective, parent, tags, NULL);
}

class IntegerWidget::Data
{
public:

    typedef std::map< int32_t, std::string > suffixMap_t;

    suffixMap_t suffix;

    int validMinimum;
    int validMaximum;

    QString tags;
};

IntegerWidget::IntegerWidget(TypeDescriptor_ptr reflective,
        QWidget * parent, 
        const QString& tags,
        TypeDescriptor_ptr enumDescriptor) :
    QSpinBox(parent), ReflectiveWidget(reflective), 
    m_data(new Data)
{
    m_data->validMinimum = std::numeric_limits< int >::min();
    m_data->validMaximum = std::numeric_limits< int >::max();

    using namespace gsim::core;

    const descriptor_type type = reflective->get_type();

    switch(type)
    {
        case TYPE_OCTET:
            setRange(std::numeric_limits< unsigned char >::min(),
                    std::numeric_limits< unsigned char >::max());
            break;
        case TYPE_CHAR:
            setRange(std::numeric_limits< char >::min(),
                    std::numeric_limits< char >::max());
            break;
        case TYPE_SHORT:
            setRange(std::numeric_limits< short >::min(),
                    std::numeric_limits< short >::max());
            break;
        case TYPE_USHORT:
            setRange(std::numeric_limits< unsigned short >::min(),
                    std::numeric_limits< unsigned short >::max());
            break;
        case TYPE_ENUM:
        case TYPE_LONG:
            setRange(std::numeric_limits< int32_t >::min(),
                    std::numeric_limits< int32_t >::max());
            break;
            // TODO custom widget
        case TYPE_ULONG:
            setRange(std::numeric_limits< uint32_t >::min(),
                    std::numeric_limits< int32_t >::max());
            break;
        case TYPE_LONGLONG:
            setRange(std::numeric_limits< int32_t >::min(),
                    std::numeric_limits< int32_t >::max());
            break;
        case TYPE_ULONGLONG:
            setRange(std::numeric_limits< uint32_t >::min(),
                    std::numeric_limits< int32_t >::max());
            break;
        default:
            break;
    }

    if (!enumDescriptor && reflective->is_enum())
    {
        enumDescriptor = reflective;
    }

    if (enumDescriptor)
    {
        const unsigned int count = enumDescriptor->get_children_count();
        Holder dummy;

        for (unsigned int i = 0; i < count; i++) 
        {
            Holder tmp (enumDescriptor->get_child_value(dummy, i));
            const char * name (enumDescriptor->get_child_name(i));

            m_data->suffix.insert(std::make_pair(
                        tmp.to_value< int32_t >(), name));
        }

        connect(this, SIGNAL(valueChanged(int)), this,
                SLOT(updateSuffix(int)));

        updateSuffix(value());
    }

    setTags(tags);

    connect(this, SIGNAL(valueChanged(int)), this,
                SLOT(checkValidValue(int)));

    // Actualiza el color de fondo actual
    checkValidValue(value());
}

IntegerWidget::~IntegerWidget()
{
    delete m_data;
}

void IntegerWidget::toHolder(Holder& holder) 
{
    using namespace gsim::core;

    const descriptor_type type = m_reflective->get_type();

    int value_ = value();

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
        case TYPE_ENUM:
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

void IntegerWidget::fromHolder(Holder& holder)
{
    using namespace gsim::core;

    const descriptor_type type = m_reflective->get_type();

    switch(type)
    {
        case TYPE_OCTET:
            setValue(holder.to_value< unsigned char >());
            break;
        case TYPE_CHAR:
            setValue(holder.to_value< char >());
            break;
        case TYPE_SHORT:
            setValue(holder.to_value< short >());
            break;
        case TYPE_USHORT:
            setValue(holder.to_value< unsigned short >());
            break;
        case TYPE_ENUM:
        case TYPE_LONG:
            setValue(holder.to_value< int32_t >());
            break;
        case TYPE_ULONG:
            setValue(holder.to_value< uint32_t >());
            break;
        case TYPE_LONGLONG:
            setValue(holder.to_value< int64_t >());
            break;
        case TYPE_ULONGLONG:
            setValue(holder.to_value< uint64_t >());
            break;
        default:
            break;
    }
}

void IntegerWidget::save(QVariant& settings)
{
    settings = value();
}

void IntegerWidget::load(const QVariant& settings)
{
    if (settings.canConvert< int >())
    {
        setValue(settings.toInt());
    }
}

void IntegerWidget::_setReadOnly(bool readOnly)
{
    setReadOnly(readOnly);
}

void IntegerWidget::updateSuffix(int value)
{
    Data::suffixMap_t::const_iterator it = 
        m_data->suffix.find(value);

    if (it != m_data->suffix.end())
    {
        setSuffix(QString(" (%1)").arg(it->second.c_str()));
    }
    else
    {
        setSuffix(QString());
    }
}

void IntegerWidget::checkValidValue(int value)
{
    bool valid = (value >= m_data->validMinimum) &&
        (value <= m_data->validMaximum);

    if (valid)
    {
       setStyleSheet("QSpinBox { background-color: white; }"); 
    }
    else
    {
       setStyleSheet("QSpinBox { background-color: red; }"); 
    }
}

int IntegerWidget::validMinimum() const
{
    return m_data->validMinimum;
}

int IntegerWidget::validMaximum() const
{
    return m_data->validMaximum;
}

void IntegerWidget::setValidMinimum(int m)
{
    m_data->validMinimum = m;

    checkValidValue(value());
}

void IntegerWidget::setValidMaximum(int m)
{
    m_data->validMaximum = m;

    checkValidValue(value());
}

QString IntegerWidget::tags() const
{
    return m_data->tags;
}

void IntegerWidget::setTags(const QString& tags)
{
    m_data->tags = tags;
    m_data->suffix.clear();

    // Etiquetas para los valores
    // No conviene usarlo junto con enumerado
    if (!tags.isEmpty())
    {
        const QStringList tagList (tags.split(';'));

        int lastValue = 0;
        for (QStringList::const_iterator it = tagList.begin(); 
                it != tagList.end(); ++it)
        {
            const QStringList itemList (it->split(':'));

            switch (itemList.size())
            {
                case 1:
                    m_data->suffix.insert(std::make_pair(lastValue,
                                itemList.at(0).toStdString()));
                case 0:
                    lastValue++;
                    break;
                case 2:
                    lastValue = itemList.at(0).toInt();
                    m_data->suffix.insert(std::make_pair(lastValue,
                                itemList.at(1).toStdString()));
                    lastValue++;
                default:
                    break;
            }
        }

        connect(this, SIGNAL(valueChanged(int)), this,
                    SLOT(updateSuffix(int)));

        // Actualiza el sufijo actual
        updateSuffix(value());
    }
}


