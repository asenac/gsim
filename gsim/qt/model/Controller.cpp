// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * Controller.cpp
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

#include "Controller.hpp"
#include <gsim/qt/initialize.hpp>
#include <gsim/qt/tools/Engine.hpp>

using namespace gsim::qt;

class Controller::Data
{
public:

    QThread thread;

    typedef std::map< Connection *, Connection_ptr > connections_t;
    connections_t connections;

    typedef std::map< QString, Connection_ptr > connections_by_name_t;
    connections_by_name_t connectionsByName;
};

Controller::Controller(QObject * parent) :
    QObject(parent), m_data(new Data)
{
    ::gsim::qt::initialize();

    connect(this,
            SIGNAL(messageReceived(Connection_ptr, Message_ptr)),
            Engine::instance()->inputMessageProcessorManager(), 
            SLOT(processMessage(Connection_ptr, Message_ptr)));

    connect(this,
            SIGNAL(messageSent(Connection_ptr, Message_ptr)),
            Engine::instance()->outputMessageProcessorManager(), 
            SLOT(processMessage(Connection_ptr, Message_ptr)));
}

Controller::~Controller()
{
    m_data->thread.quit();
    m_data->thread.wait();

    // Deja al shared_ptr hacer su trabajo
    for (Data::connections_t::iterator it =
            m_data->connections.begin(); 
            it != m_data->connections.end(); ++it) 
    {
        it->second->setParent(NULL);
    }

    delete m_data;
}

void Controller::start()
{
    m_data->thread.start();
    moveToThread(&m_data->thread);
}

void Controller::createConnection(ConnectionConfig_ptr cfg)
{
    Connection_ptr con;

    if ((con = doCreateConnection(cfg)))
    {
        addConnection(con);
    }
}

void Controller::addConnection(Connection_ptr con)
{
    if (m_data->connectionsByName.find(con->name()) != 
            m_data->connectionsByName.end())
        throw "There is already a connection with the same name.";

    con->setParent(this);

    m_data->connections[con.get()] = con;
    m_data->connectionsByName[con->name()] = con;

    connect(con.get(), SIGNAL(messageSent(Message_ptr)),
            this, SLOT(messageSent(Message_ptr)));
    connect(con.get(), SIGNAL(messageReceived(Message_ptr)),
            this, SLOT(messageReceived(Message_ptr)));

    connect(con.get(), SIGNAL(error(const QString&)),
            this, SLOT(notifyError(const QString&)));

    emit connectionCreated(con);
}

void Controller::removeConnection(Connection_ptr con)
{
    m_data->connectionsByName.erase(con->name());
    m_data->connections.erase(con.get());

    disconnect(con.get(), SIGNAL(messageSent(Message_ptr)),
            this, SLOT(messageSent(Message_ptr)));
    disconnect(con.get(), SIGNAL(messageReceived(Message_ptr)),
            this, SLOT(messageReceived(Message_ptr)));

    disconnect(con.get(), SIGNAL(error(const QString&)),
            this, SLOT(notifyError(const QString&)));

    emit connectionDeleted(con);
}

void Controller::messageSent(Message_ptr msg)
{
    Connection * con = qobject_cast< Connection * >(sender());

    if (con)
    {
        Data::connections_t::iterator it = m_data->connections.find(con);

        if (it != m_data->connections.end())
        {
            emit messageSent(it->second, msg);
        }
    }
}

void Controller::messageReceived(Message_ptr msg)
{
    Connection * con = qobject_cast< Connection * >(sender());

    if (con)
    {
        Data::connections_t::iterator it = m_data->connections.find(con);

        if (it != m_data->connections.end())
        {
            emit messageReceived(it->second, msg);
        }
    }
}

Connection_ptr Controller::doCreateConnection(ConnectionConfig_ptr)
{
    return Connection_ptr();
}

Connection_ptr Controller::getConnection(const QString& name) const
{
    Data::connections_by_name_t::const_iterator it = 
        m_data->connectionsByName.find(name);

    if (it != m_data->connectionsByName.end())
        return it->second;

    return Connection_ptr();
}

void Controller::notifyError(const QString& err)
{
    QObject * obj = sender();
    Connection * con = NULL;

    if (obj && (con = dynamic_cast< Connection * >(obj)))
    {
        emit error(QString("Connection %1 - %2")
                .arg(con->name())
                .arg(err));
    }
    else
    {
        emit error(err);
    }
}

