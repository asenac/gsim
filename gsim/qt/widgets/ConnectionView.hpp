// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * ConnectionView.hpp
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

#ifndef GSIM_QT_WIDGETS_CONNECTIONVIEW_HPP
#define GSIM_QT_WIDGETS_CONNECTIONVIEW_HPP

#include <boost/shared_ptr.hpp>
#include <gsim/qt/model/Connection.hpp>

class QMenu;
class QWidget;
class QDockWidget;

namespace gsim 
{
namespace qt 
{

class ConnectionView : public QObject
{
    Q_OBJECT
public:
    ConnectionView(Connection_ptr con, QObject * parent = 0);
    ~ConnectionView();

    Connection_ptr connection() const;

    QMenu * menu() const;
    QWidget * statusWidget() const;
    QWidget * connectionWidget() const;
    QDockWidget * connectionDockWidget() const;

    void load(const QVariant& settings);
    void save(QVariant& settings);

public slots:

    void showSenderDialog(int);
    void showConfig();

    void loadStatus();
    void saveStatus();

protected:
    struct Data;
    Data * m_data;
};

typedef boost::shared_ptr< ConnectionView > ConnectionView_ptr;

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_WIDGETS_CONNECTIONVIEW_HPP */

