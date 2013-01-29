// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * ScriptEngine.cpp
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

#include "ScriptEngine.hpp"
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

using namespace gsim::qt;

class ScriptEngine::Data : public QScriptEngine
{
public:

    Data(ScriptEngine * this__) :
        QScriptEngine(this__),
        this_(this__), work(ioService),
        thread(boost::bind(&boost::asio::io_service::run, &ioService))
    {
    }

    ~Data()
    {
        ioService.stop();
        thread.join();
    }

    void runCode(const QString& code)
    {
        // this_->emit error(code);
        
        evaluate(code);
    }

    void connectionCreated(Connection_ptr c)
    {
    }

    void connectionDeleted(Connection_ptr c)
    {
    }

    void messageSent(Connection_ptr c, Message_ptr m)
    {
    }

    void messageReceived(Connection_ptr c, Message_ptr m)
    {
    }

    ScriptEngine * this_;
    boost::asio::io_service ioService;
    boost::asio::io_service::work work;
    boost::thread thread;
};

ScriptEngine::ScriptEngine(QObject * parent) :
    QObject(parent), m_data(new Data(this))
{
}

ScriptEngine::~ScriptEngine()
{
    delete m_data;
}

void ScriptEngine::run(const QString& code)
{
    m_data->ioService.post(boost::bind(&Data::runCode,
                m_data, code));
}

void ScriptEngine::connectionCreated(Connection_ptr c)
{
    m_data->ioService.post(boost::bind(&Data::connectionCreated,
                m_data, c));
}

void ScriptEngine::connectionDeleted(Connection_ptr c)
{
    m_data->ioService.post(boost::bind(&Data::connectionDeleted,
                m_data, c));
}

void ScriptEngine::messageSent(Connection_ptr c, Message_ptr m)
{
    m_data->ioService.post(boost::bind(&Data::messageSent,
                m_data, c, m));
}

void ScriptEngine::messageReceived(Connection_ptr c, Message_ptr m)
{
    m_data->ioService.post(boost::bind(&Data::messageReceived,
                m_data, c, m));
}

