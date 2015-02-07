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

}  // namespace

GenericConnection::GenericConnection(QObject* parent)
    : Connection(ConnectionDescriptor_ptr(), parent), m_data(new Data(this))
{
}

GenericConnection::GenericConnection(ConnectionDescriptor_ptr descriptor,
                                     QObject* parent)
    : Connection(descriptor, parent), m_data(new Data(this))
{
}

GenericConnection::~GenericConnection()
{
    m_data->connection.reset();
    delete m_data;
}

std::size_t GenericConnection::processData(const char* data, std::size_t size)
{
    return size;
}

void GenericConnection::send(const char* data, std::size_t size)
{
    if (!m_data->connection.isNull())
    {
        UDPConnectionConfig* udpCfg =
            dynamic_cast<UDPConnectionConfig*>(m_data->cfg.get());

        if (udpCfg)
        {
            // XXX cannot use write() method for UDP sockets due to
            // the bug in connectToHost that resets the local port
            // specified by bind()
            QUdpSocket* socket =
                static_cast<QUdpSocket*>(m_data->connection.data());
            const QHostAddress remoteHost(udpCfg->remoteHost.c_str());
            socket->writeDatagram(data, size, remoteHost, udpCfg->remotePort);
        }
        else
        {
            m_data->connection->write(data, size);
        }
    }
}

bool GenericConnection::applyConfig(ConnectionConfig_ptr cfg)
{
    return m_data->applyConfig(cfg);
}

bool GenericConnection::Data::applyConfig(ConnectionConfig_ptr cfg_)
{
    bool res = false;

    if (!cfg_)
    {
        connection.reset();
        this_->setStatus(kStatusDisconnected);

        res = true;
    }
    else  // if (connection.isNull())
    {
        UDPConnectionConfig* udpCfg =
            dynamic_cast<UDPConnectionConfig*>(cfg_.get());

        if (udpCfg)
        {
            QUdpSocket* socket = new QUdpSocket(this);
            connection.reset(socket);

            const QHostAddress localHost(udpCfg->localHost.c_str());
            if (!socket->bind(localHost, udpCfg->localPort))
            {
                this_->setStatus(kStatusDisconnected);
                connection.reset();

                emit this_->error(QString("Cannot use local UDP endpoint %1:%2")
                                      .arg(localHost.toString())
                                      .arg(udpCfg->localPort));
            }
            else
            {
                // XXX connectToHost resets local portd due to a bug
                // https://bugreports.qt.io/browse/QTBUG-26538
                // const QHostAddress remoteHost(udpCfg->remoteHost.c_str());
                // socket->connectToHost(remoteHost, udpCfg->remotePort);

                connect(socket,
                        SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                        this, SLOT(stateChanged(QAbstractSocket::SocketState)));

                connect(socket, SIGNAL(readyRead()), this,
                        SLOT(readPendingDataUDP()));

                this_->setStatus(kStatusConnected);

                res = true;
            }
        }
        else
        {
            ::gsim::qt::TCPConnectionConfig* tcpCfg =
                dynamic_cast< ::gsim::qt::TCPConnectionConfig*>(cfg_.get());

            if (tcpCfg)
            {
                // TODO
            }
        }
    }

    if (res)
        cfg = cfg_;
    else
        cfg.reset();

    return res;
}

void GenericConnection::Data::readPendingDataUDP()
{
    if (!connection.isNull())
    {
        QUdpSocket* socket = static_cast<QUdpSocket*>(connection.data());

        while (socket->hasPendingDatagrams())
        {
            int offset = buffer.size();
            int avail = socket->pendingDatagramSize();

            buffer.resize(offset + avail);

            socket->readDatagram(buffer.data() + offset, avail);

            // std::size_t consume =
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
