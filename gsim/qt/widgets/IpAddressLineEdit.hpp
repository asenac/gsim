// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * IpAddressLineEdit.hpp
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

#ifndef GSIM_IPADDRESSLINEEDIT_HPP
#define GSIM_IPADDRESSLINEEDIT_HPP

#include <QLineEdit>
#include <QRegExpValidator>

namespace gsim 
{
namespace qt 
{

class IpAddressLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    IpAddressLineEdit(QWidget * parent = 0);
    virtual ~IpAddressLineEdit();

private slots:

    void textHasChanged();

private:

    QRegExpValidator * m_validator;
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_IPADDRESSLINEEDIT_HPP */

