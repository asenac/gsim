// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * StringWidget.cpp
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

#include "StringWidget.hpp"
#include <cassert>
#include <cstring>
#include <algorithm>

using namespace gsim::qt;

// String Widget

StringWidget::StringWidget(TypeDescriptor_ptr reflective,
        QWidget * parent) :
    QLineEdit(parent), ReflectiveWidget(reflective)
{
    using namespace gsim::core;

    bool array = reflective->get_type() == TYPE_ARRAY 
        && reflective->get_slice()->get_type() == TYPE_CHAR;

    assert(reflective->get_type() == TYPE_STRING || array);

    if (array)
    {
        Holder dummy;
        unsigned length = reflective->get_length(dummy);

        setMaxLength((int) length);
    }
}

StringWidget::~StringWidget()
{
}

void StringWidget::toHolder(Holder& holder)
{
    const std::string str(text().toStdString());

    m_reflective->from_string(holder, str);
}

void StringWidget::fromHolder(Holder& holder)
{
    const std::string str(
            m_reflective->to_string(holder));
    setText(str.c_str());
}

QString StringWidget::value() const 
{
    return text();
}

void StringWidget::setValue(const QString& value) 
{
    setText(value);
}

void StringWidget::save(QVariant& settings)
{
    settings = text();
}

void StringWidget::load(const QVariant& settings)
{
    if (settings.canConvert< QString >())
    {
        setText(settings.toString());
    }
}

void StringWidget::_setReadOnly(bool readOnly)
{
    setReadOnly(readOnly);
}

