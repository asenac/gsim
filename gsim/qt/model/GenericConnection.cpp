// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * GenericConnection.cpp
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

#include "GenericConnection.hpp"
#include "GenericConnection_Data.hpp"
#include <iostream>

using namespace gsim::qt;

namespace
{

ConnectionStatus translate(QAbstractSocket::SocketState st)
{
    ConnectionStatus res = kStatusDisconnected;

    switch (st)
    {
        case QAbstractSocket::ConnectingState:
        case QAbstractSocket::BoundState:
            res = kStatusListening;
            break;
        case QAbstractSocket::ConnectedState:
            res = kStatusConnected;
            break;
        default:
            break;
    }

    return res;
}

} // namespace

GenericConnection::GenericConnection(QObject * parent) :
    Connection(ConnectionDescriptor_ptr(), parent), m_data(new Data(this))
{
}

GenericConnection::GenericConnection(ConnectionDescriptor_ptr descriptor,
        QObject * parent) :
    Connection(descriptor, parent), m_data(new Data(this))
{
}

GenericConnection::~GenericConnection()
{
    m_data->connection.reset();
    delete m_data;
}

std::size_t GenericConnection::processData(const char * data, std::size_t size)
{
    return size;
}

void GenericConnection::send(const char * data, std::size_t size)
{
    if (!m_data->connection.isNull())
    {
        m_data->connection->write(data, size);
    }
}

bool GenericConnection::applyConfig(ConnectionConfig_ptr cfg)
{
    return m_data->applyConfig(cfg);
}

bool GenericConnection::Data::applyConfig(ConnectionConfig_ptr cfg)
{
    bool res = false;

    if (!cfg)
    {
        connection.reset();
        this_->setStatus( ::gsim::qt::kStatusDisconnected);

        res = true;
    }
    else // if (connection.isNull())
    {
        ::gsim::qt::UDPConnectionConfig * udpCfg =
            dynamic_cast< ::gsim::qt::UDPConnectionConfig * >(cfg.get());

        if (udpCfg)
        {
            QUdpSocket* socket = new QUdpSocket(this);
            connection.reset(socket);

            const QHostAddress localHost(udpCfg->localHost.c_str());
            const QHostAddress remoteHost(udpCfg->remoteHost.c_str());

            socket->bind(localHost, udpCfg->localPort);
            connection->connectToHost(remoteHost, udpCfg->remotePort);

            connect(connection.data(),
                    SIGNAL(stateChanged(QAbstractSocket::SocketState)), this,
                    SLOT(stateChanged(QAbstractSocket::SocketState)));

            connect(connection.data(), SIGNAL(readyRead()), this,
                    SLOT(readPendingDataUDP()));

            this_->setStatus(translate(connection->state()));

            res = true;
        }
        else
        {
            ::gsim::qt::TCPConnectionConfig * tcpCfg =
                dynamic_cast< ::gsim::qt::TCPConnectionConfig * >(cfg.get());

            if (tcpCfg)
            {
                // TODO
            }
        }
    }

    return res;
}

void GenericConnection::Data::readPendingDataUDP()
{
    std::cout << __FUNCTION__ << std::endl;
    if (!connection.isNull())
    {
        QUdpSocket * socket =
            static_cast< QUdpSocket * >(connection.data());

        while (socket->hasPendingDatagrams())
        {
            int offset = buffer.size();
            int avail = socket->pendingDatagramSize();

            buffer.resize(offset + avail);

            socket->readDatagram(buffer.data() + offset, avail);

            //std::size_t consume =
                this_->processData(buffer.data(), buffer.size());

            buffer.resize(0);
        }
    }
}

void GenericConnection::Data::stateChanged(
    QAbstractSocket::SocketState socketState)
{
    this_->setStatus(translate(socketState));
}
