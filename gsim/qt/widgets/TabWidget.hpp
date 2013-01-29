// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * TabWidget.hpp
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

#ifndef GSIM_QT_TABWIDGET_HPP
#define GSIM_QT_TABWIDGET_HPP

#include <QTabWidget>
#include <gsim/qt/export.hpp>

namespace gsim 
{
namespace qt 
{

class GSIM_QT_DECLSPEC TabWidget : public QTabWidget
{
    Q_OBJECT
public:
    TabWidget(QWidget * parent = 0);
    virtual ~TabWidget();

    QTabBar * tabBar() const;
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_TABWIDGET_HPP */

