// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * GenericConnection_Data.cpp
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

#ifndef GSIM_QT_GENERICCONNECTION_DATA_HPP
#define GSIM_QT_GENERICCONNECTION_DATA_HPP

#include "GenericConnection.hpp"
#include <QObject>
#include <QScopedPointer>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QUdpSocket>
#include <QByteArray>
#include <cassert>

namespace gsim
{
namespace qt
{

class GenericConnection::Data : public QObject
{
    Q_OBJECT
public:
    Data(GenericConnection* parent) : QObject(parent), this_(parent)
    {
        assert(parent);
    }

    ~Data() {}

    bool applyConfig(ConnectionConfig_ptr cfg);

public slots:

    void readPendingDataUDP();

    void stateChanged(QAbstractSocket::SocketState socketState);

protected:

    friend class GenericConnection;

    typedef QScopedPointer< QAbstractSocket > socket_ptr;
    typedef QScopedPointer< QTcpServer > server_ptr;

    GenericConnection * this_;

    socket_ptr connection;
    ConnectionConfig_ptr cfg;

    QByteArray buffer;
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_GENERICCONNECTION_DATA_HPP */
