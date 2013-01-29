// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * DateTimeWidget.cpp
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

#include "DateTimeWidget.hpp"
#include <cassert>
#include <gsim/core/descriptor_utils.hpp>

using namespace gsim::qt;

// IPAddress widget

class DateTimeWidget::Data
{
public:

    bool hasDay;
    unsigned int dayIdx;

    bool hasMonth;
    unsigned int monthIdx;

    bool hasYear;
    unsigned int yearIdx;

    bool hasHour;
    unsigned int hourIdx;

    bool hasMinute;
    unsigned int minuteIdx;
    
    bool hasSecond;
    unsigned int secondIdx;
};

QWidget * DateTimeWidget::create(TypeDescriptor_ptr reflective,
        QWidget * parent)
{
    return new DateTimeWidget(reflective, NULL, parent);
}

QWidget * DateTimeWidget::createWithFormat(TypeDescriptor_ptr reflective,
        const char * format,
        QWidget * parent)
{
    return new DateTimeWidget(reflective, format, parent);
}

DateTimeWidget::DateTimeWidget(TypeDescriptor_ptr reflective,
        const char * format,
        QWidget * parent) :
    QDateTimeEdit(parent), ReflectiveWidget(reflective), m_data(new Data)
{
    using namespace gsim::core;

    assert(
            // 4 octetos
            reflective->get_type() == TYPE_ULONG 

            // Estructura 
            || (reflective->get_type() == TYPE_STRUCT)

            // String
            || (reflective->get_type() == TYPE_STRING));

    if (format) setDisplayFormat(format);

    if (reflective->get_type() == TYPE_STRUCT)
    {
        const unsigned int count = reflective->get_children_count();

        m_data->dayIdx = utils::nearest_child(reflective, "day");
        m_data->monthIdx = utils::nearest_child(reflective, "month");
        m_data->yearIdx = utils::nearest_child(reflective, "year");
        m_data->hourIdx = utils::nearest_child(reflective, "hour");
        m_data->minuteIdx = utils::nearest_child(reflective, "minute");
        m_data->secondIdx = utils::nearest_child(reflective, "second");

        m_data->hasDay = (m_data->dayIdx < count);
        m_data->hasMonth = (m_data->monthIdx < count);
        m_data->hasYear = (m_data->yearIdx < count);
        m_data->hasHour = (m_data->hourIdx < count);
        m_data->hasMinute = (m_data->minuteIdx < count);
        m_data->hasSecond = (m_data->secondIdx < count);
    }
}

DateTimeWidget::~DateTimeWidget()
{
    delete m_data;
}

#define _GSIM_DATE_TIME_IMPL(x, y, z) \
    if (m_data->has ## x) \
    { \
        descriptor_ptr d = m_reflective->get_child(m_data->y ## Idx); \
        Holder c = m_reflective->get_child_value(holder, m_data->y ## Idx); \
        switch(d->get_type()) \
        { \
            case TYPE_ULONG: \
                c.to_value< uint32_t >() = z; \
                break; \
            case TYPE_LONG: \
                c.to_value< int32_t >() = z; \
                break; \
            case TYPE_ULONGLONG: \
                c.to_value< uint64_t >() = z; \
                break; \
            case TYPE_LONGLONG: \
                c.to_value< int64_t >() = z; \
                break; \
            case TYPE_OCTET: \
                c.to_value< unsigned char >() = ((int32_t) z) % 100; \
                break; \
            case TYPE_CHAR: \
                c.to_value< char >() = ((int32_t) z) % 100; \
                break; \
            case TYPE_FLOAT: \
                c.to_value< float >() = (float) z; \
                break; \
            case TYPE_DOUBLE: \
                c.to_value< double >() = (double) z; \
                break; \
            default: \
                break; \
        } \
    } \
    /***/

void DateTimeWidget::toHolder(Holder& holder) 
{
    using namespace gsim::core;

    const descriptor_type type = m_reflective->get_type();

    const QDateTime dateTime_ = dateTime();
    const QDate date_ = dateTime_.date();
    const QTime time_ = dateTime_.time();

    switch(type)
    {
        case TYPE_STRUCT:
            {
                _GSIM_DATE_TIME_IMPL(Day, day, date_.day());
                _GSIM_DATE_TIME_IMPL(Month, month, date_.month());
                _GSIM_DATE_TIME_IMPL(Year, year, date_.year());
                _GSIM_DATE_TIME_IMPL(Hour, hour, time_.hour());
                _GSIM_DATE_TIME_IMPL(Minute, minute, time_.minute());

                const float seconds = (float) time_.second() + 
                    ((float) time_.msec()) / 1000;

                _GSIM_DATE_TIME_IMPL(Second, second, seconds);
            }
            break;
        case TYPE_ULONG:
            break;
        case TYPE_STRING:
            break;
        default:
            break;
    }
}

#undef _GSIM_DATE_TIME_IMPL
#define _GSIM_DATE_TIME_IMPL(x, y, z) \
    if (m_data->has ## x) \
    { \
        descriptor_ptr d = m_reflective->get_child(m_data->y ## Idx); \
        Holder c = m_reflective->get_child_value(holder, m_data->y ## Idx); \
        switch(d->get_type()) \
        { \
            case TYPE_ULONG: \
                z (c.to_value< uint32_t >()); \
                break; \
            case TYPE_LONG: \
                z (c.to_value< int32_t >()); \
                break; \
            case TYPE_ULONGLONG: \
                z (c.to_value< uint64_t >()); \
                break; \
            case TYPE_LONGLONG: \
                z (c.to_value< int64_t >()); \
                break; \
            case TYPE_OCTET: \
                z (c.to_value< unsigned char >()); \
                break; \
            case TYPE_CHAR: \
                z (c.to_value< char >()); \
                break; \
            case TYPE_FLOAT: \
                z (c.to_value< float >()); \
                break; \
            case TYPE_DOUBLE: \
                z (c.to_value< double >()); \
                break; \
            default: \
                break; \
        } \
    } \
    /***/


void DateTimeWidget::fromHolder(Holder& holder)
{
    using namespace gsim::core;

    const descriptor_type type = m_reflective->get_type();

    switch(type)
    {
        case TYPE_ULONG:
            break;
        case TYPE_STRING:
            {
            }
            break;
        case TYPE_STRUCT:
            {
                int day = 0;
                int month = 0;
                int year = 0;

                _GSIM_DATE_TIME_IMPL(Day, day, day = (int));
                _GSIM_DATE_TIME_IMPL(Month, month, month = (int));
                _GSIM_DATE_TIME_IMPL(Year, year, year = (int));

                const QDate date_(year, month, day);
                setDate(date_);

                int hour = 0;
                int minute = 0;
                float seconds = 0.0;

                _GSIM_DATE_TIME_IMPL(Hour, hour, hour = (int));
                _GSIM_DATE_TIME_IMPL(Minute, minute, minute = (int));
                _GSIM_DATE_TIME_IMPL(Second, second, seconds = (float));

                int msec = (int) (seconds * 1000);
                const QTime time_(hour, minute, msec / 1000, msec % 1000);
                setTime(time_);
            }
            break;
        default:
            break;
    }
}

void DateTimeWidget::save(QVariant& settings)
{
    settings = dateTime().toString();
}

void DateTimeWidget::load(const QVariant& settings)
{
    setDateTime(QDateTime::fromString(settings.toString()));
}

void DateTimeWidget::_setReadOnly(bool readOnly)
{
    setReadOnly(readOnly);
}

