// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * Connection.cpp
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

#include "Connection.hpp"
#include <gsim/qt/initialize.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <gsim/qt/model/Controller.hpp>
#include <fstream>

#include <gsim/core/event.hpp>
#include <gsim/qt/model/StatusManager.hpp>

using namespace gsim::qt;

typedef boost::shared_lock< boost::shared_mutex > shared_lock;
typedef boost::unique_lock< boost::shared_mutex > unique_lock;

Connection::Functor::~Functor()
{
}

ConnectionConfig::~ConnectionConfig() {}

class Connection::Data
{
public:

    Data(ConnectionDescriptor_ptr descriptor_) : 
        status(kStatusDisconnected), 
        descriptor(descriptor_),
        name("Connection"),
        type("TCP"),
        statusManager(NULL)
    {
        ::gsim::qt::initialize();

        if (!descriptor)
            descriptor.reset(new ::gsim::core::connection_descriptor);
    }

    boost::shared_mutex statusMutex;
    ConnectionStatus status;

    ::gsim::core::connection_descriptor_ref descriptor;

    boost::shared_mutex nameMutex;
    QString name;

    boost::shared_mutex typeMutex;
    QString type;

    boost::shared_mutex configMutex;
    ConnectionConfig_ptr config;

    boost::shared_mutex tableMutex;
    typedef std::map< core::descriptor_ptr, Functor_ptr > table_t;
    table_t table;

    boost::shared_mutex statusManagerMutex;
    StatusManager * statusManager;
};

Connection::Connection(
        ConnectionDescriptor_ptr descriptor,
        QObject * parent) :
    QObject(parent), m_data(new Data(descriptor))
{
}

Connection::~Connection()
{
    delete m_data;
}

Controller * Connection::controller() const
{
    return dynamic_cast< Controller * >(parent());
}

void Connection::setStatusManager(StatusManager * manager)
{
    unique_lock lock(m_data->statusManagerMutex);
    m_data->statusManager = manager;
}

StatusManager * Connection::statusManager() const
{
    shared_lock lock(m_data->statusManagerMutex);
    return m_data->statusManager;
}
    
void Connection::sendMessage(Message_ptr msg)
{
    if (status() == kStatusConnected && doSendMessage(msg))
    {
        emit messageSent(msg);
    }
}

void Connection::setConfig(ConnectionConfig_ptr cfg)
{
    unique_lock lock(m_data->configMutex);

    if (applyConfig(cfg))
    {
        m_data->config = cfg;
        emit configChanged(cfg);
    }
}

void Connection::setConfig(ConnectionConfig * cfg)
{
    setConfig(ConnectionConfig_ptr(cfg));
}

void Connection::forwardTo(Connection_ptr con)
{
    connect(this, SIGNAL(messageReceived(Message_ptr)),
            con.get(), SLOT(sendMessage(Message_ptr)));
}

/*
bool Connection::applyConfig(ConnectionConfig_ptr cfg)
{
    std::cout << __FUNCTION__ << " fake" << std::endl;
    return true;
}
*/
void Connection::notifyMessageReceived(Message_ptr msg)
{
    emit messageReceived(msg);
}

void Connection::setName(const QString& name)
{
    {
        unique_lock lock(m_data->nameMutex);

        m_data->name = name;
    }

    emit nameChanged(name);
}

QString Connection::name() const
{
    shared_lock lock(m_data->nameMutex);
    return m_data->name;
}

void Connection::setType(const QString& type)
{
    {
        unique_lock lock(m_data->typeMutex);

        m_data->type = type;
    }

    emit typeChanged(type);
}

QString Connection::type() const
{
    shared_lock lock(m_data->typeMutex);
    return m_data->type;
}

ConnectionStatus Connection::status() const
{
    shared_lock lock(m_data->statusMutex);
    return m_data->status;
}

void Connection::setStatus(ConnectionStatus status_)
{
    if (status_ != status())
    {
        unique_lock lock(m_data->statusMutex);

        m_data->status = status_;

        emit statusChanged(status_);
    }
}

ConnectionDescriptor_ptr Connection::descriptor() const
{
    return m_data->descriptor;
}

ConnectionConfig_ptr Connection::config() const
{
    shared_lock lock(m_data->configMutex);

    return m_data->config;
}

bool Connection::doSendMessage(Message_ptr msg)
{
    shared_lock lock(m_data->tableMutex);
    core::holder h(msg->holder());

    // Busca el functor en la tabla
    Data::table_t::iterator it = 
        m_data->table.find(h.get_type_descriptor());

    if (it != m_data->table.end())
    {
        return (*it->second)(h);
    }

    return false;
}

void Connection::doRegisterMethod(core::descriptor_ptr descriptor, 
        Functor_ptr f) 
{
    unique_lock lock(m_data->tableMutex);
    m_data->table[descriptor->get_type_descriptor()] = f;
}

bool Connection::sendStatus(const QString& name)
{
    shared_lock lock(m_data->statusManagerMutex);

    if (m_data->statusManager && status() == kStatusConnected)
    {
        Holder h(m_data->statusManager->status(name));

        if (h.is_valid())
        {
            Message_ptr message(new ::gsim::core::message(
                        name.toStdString(), h));

            bool res = doSendMessage(message);

            if (res) emit messageSent(message);

            return res;
        }
    }

    return false;
}

