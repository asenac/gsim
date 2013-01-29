// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * BigTextWidget.cpp
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

#include "BigTextWidget.hpp"
#include <cassert>
#include <cstring>
#include <algorithm>

using namespace gsim::qt;

// String Widget

BigTextWidget::BigTextWidget(TypeDescriptor_ptr reflective,
        QWidget * parent) :
    QTextEdit(parent), ReflectiveWidget(reflective)
{
    using namespace gsim::core;

    setBigWidget(true);
    setAcceptRichText(false);

    // Elimina warning.
#ifndef NDEBUG
    bool array = (reflective->get_type() == TYPE_ARRAY
        && reflective->get_slice()->get_type() == TYPE_CHAR);

    assert(reflective->get_type() == TYPE_STRING || array);
#endif

    // No existe método para establecer longitud
    /*
    if (array)
    {
        Holder dummy;
        unsigned length = reflective->get_length(dummy);

        setMaxLength((int) length);
    }
    */
}

BigTextWidget::~BigTextWidget()
{
}

void BigTextWidget::toHolder(Holder& holder)
{
    const std::string str(toPlainText().toStdString());

    m_reflective->from_string(holder, str);
}

void BigTextWidget::fromHolder(Holder& holder)
{
    const std::string str(
            m_reflective->to_string(holder));
    setPlainText(str.c_str());
}

QString BigTextWidget::value() const 
{
    return toPlainText();
}

void BigTextWidget::setValue(const QString& value) 
{
    setPlainText(value);
}

void BigTextWidget::save(QVariant& settings)
{
    settings = value();
}

void BigTextWidget::load(const QVariant& settings)
{
    if (settings.canConvert< QString >())
    {
        setPlainText(settings.toString());
    }
}

void BigTextWidget::_setReadOnly(bool readOnly)
{
    setReadOnly(readOnly);
}

QWidget * BigTextWidget::create(TypeDescriptor_ptr descriptor,
        QWidget * parent)
{
    return new BigTextWidget(descriptor, parent);
}

