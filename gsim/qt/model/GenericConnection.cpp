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
#include <boost/bind.hpp>
#include <memory>
#include <QTcpSocket>
#include <QUdpSocket>

using namespace gsim::qt;

class GenericConnection::Data
{
public:

    typedef std::auto_ptr< QAbstractSocket > socket_ptr; 

    socket_ptr connection;
};

GenericConnection::GenericConnection(QObject * parent) : 
    Connection(ConnectionDescriptor_ptr(), parent), m_data(new Data)
{
}

GenericConnection::GenericConnection(ConnectionDescriptor_ptr descriptor,
        QObject * parent) : 
    Connection(descriptor, parent), m_data(new Data)
{
}

GenericConnection::~GenericConnection()
{
    delete m_data;
}

std::size_t GenericConnection::processData(const char * data, std::size_t size)
{
    return size;
}

void GenericConnection::send(const char * data, std::size_t size)
{
    if (m_data->connection.get())
    {
        m_data->connection->write(data, size);
    }
}

bool GenericConnection::applyConfig(ConnectionConfig_ptr cfg)
{
    bool res = false;

    if (!cfg)
    {
        m_data->connection.reset();
        setStatus( ::gsim::qt::kStatusDisconnected);

        res = true;
    }
    else
    {
        ::gsim::qt::UDPConnectionConfig * udpCfg = 
            dynamic_cast< ::gsim::qt::UDPConnectionConfig * >(cfg.get());

        if (!m_data->connection.get() && udpCfg)
        {
            // res = true;
            // TODO
        }
    }

    return res;
}

