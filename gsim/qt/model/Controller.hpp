// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * Controller.hpp
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

#ifndef GSIM_QT_CONTROLLER_HPP
#define GSIM_QT_CONTROLLER_HPP

#include <QtCore>

#include <gsim/qt/model/Connection.hpp>

namespace gsim 
{
namespace qt 
{

class Controller : public QObject
{
    Q_OBJECT
public:

    Controller(QObject * parent = 0);
    virtual ~Controller();

    virtual void start();

    Connection_ptr getConnection(const QString& name) const;

public slots:

    void messageSent(Message_ptr);
    void messageReceived(Message_ptr);

    void createConnection(ConnectionConfig_ptr);

    void addConnection(Connection_ptr);
    void removeConnection(Connection_ptr);

signals:

    void connectionCreated(Connection_ptr);
    void connectionDeleted(Connection_ptr);

    void error(const QString&);
    void message(const QString&);

    void messageSent(Connection_ptr, Message_ptr);
    void messageReceived(Connection_ptr, Message_ptr);

protected slots:

    void notifyError(const QString&);

protected:

    virtual Connection_ptr doCreateConnection(ConnectionConfig_ptr);

    class Data;
    Data * m_data;
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_CONTROLLER_HPP */

