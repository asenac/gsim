// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * IpAddressLineEdit.cpp
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

#include "IpAddressLineEdit.hpp"

using namespace gsim::qt;

IpAddressLineEdit::IpAddressLineEdit(QWidget * parent) :
    QLineEdit(parent), m_validator(NULL)
{
    const QRegExp correct(
            "^[0-2]?[0-9]?[0-9]\\.[0-2]?[0-9]?[0-9]\\.[0-2]?[0-9]?[0-9]\\.[0-2]?[0-9]?[0-9]$");
    m_validator = new QRegExpValidator(correct, this);

    setText("0.0.0.0");
    setValidator(m_validator);
    setInputMask("000.000.000.000; ");
}

IpAddressLineEdit::~IpAddressLineEdit()
{
    delete m_validator;
}

void IpAddressLineEdit::textHasChanged()
{
}

