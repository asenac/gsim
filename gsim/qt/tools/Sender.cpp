// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * Sender.cpp
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

#include "Sender.hpp"
#include <boost/bind.hpp>
#include <gsim/core/event.hpp>
#include <gsim/qt/model/Connection.hpp>
#include <gsim/qt/reflective/path.hpp>
#include <gsim/qt/item/ModelNode.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <memory>
#include <iostream>
#include <exception>

using namespace gsim::qt;

// 
//
// SenderController
//
//

SenderController::SenderController() :
    m_work(m_ioService)
{
}

SenderController::~SenderController()
{
}

void SenderController::start(unsigned int numberOfThreads)
{
    // std::cout << "SenderController::" 
    //     << __FUNCTION__ << std::endl;

    for (unsigned int i = 0; i < numberOfThreads; i++) 
    {
        m_threads.create_thread(boost::bind(
                    &boost::asio::io_service::run,
                    &m_ioService));
    }
}

void SenderController::join()
{
    m_threads.join_all();
}

void SenderController::stop()
{
    m_ioService.stop();
}

// slots

void SenderController::addSender(SenderConfig_ptr cfg)
{
    Sender_ptr sender(new Sender(m_ioService, cfg));

    // Self-delete
    connect(sender.get(), 
            SIGNAL(finished(SenderConfig_ptr)),
            this,
            SLOT(deleteSender(SenderConfig_ptr)));

    // error notification
    connect(sender.get(), 
            SIGNAL(error(const QString&)),
            this,
            SIGNAL(error(const QString&)));

    m_ioService.post(
            boost::bind(&Sender::start, sender, sender));

    m_map.insert(cfg, sender);
}

void SenderController::deleteSender(SenderConfig_ptr cfg)
{
    map_t::iterator it = m_map.find(cfg);

    if (it != m_map.end())
    {
        it.value()->cancel();

        m_map.erase(it);
    }
}

// 
//
// Sender
//
//


Sender::Sender(boost::asio::io_service& ioService,
    SenderConfig_ptr config) :
    m_timer(ioService), 
    m_config(config),
    m_currentTime(0), 
    m_evaluator(config->messageDescriptor(), this)
{
    connect(this, 
            SIGNAL(sendMessage(Message_ptr)),
            config->connection().get(), 
            SLOT(sendMessage(Message_ptr)));
}

Sender::~Sender() 
{
}

void Sender::start(Sender_weak weak) 
{
    Sender_ptr ptr (weak.lock());

    if (ptr)
    {
        m_evaluator.evaluate(m_config->code());

        bool hasErr = m_evaluator.hasError();

        if (!hasErr)
        {
            // Clone the original message
            // We can't modify an emited message
            // Deep copy
            m_message.reset(m_config->message()->clone());

            // m_message is the working message
            m_evaluator.init(m_message);

            hasErr = m_evaluator.hasError();
        }

        if (!hasErr)
        {
            m_timer.expires_from_now(
                    boost::posix_time::milliseconds(
                        m_config->initDelay()));

            m_timer.async_wait(
                    boost::bind(&Sender::handleTimeout,
                        this, weak, _1));
        }
        else
        {
            m_config->notifyFinished();

            emit finished(m_config);

            emit error(m_evaluator.error());
        }
    }
}

void Sender::cancel() 
{
    m_timer.cancel();
}

void Sender::process()
{
    // preFunc
    m_evaluator.pre(m_message);
    if (m_evaluator.hasError()) throw m_evaluator.error();

    // processors
    typedef QList< MessageProcessor_ptr > processors_t;
    const processors_t& processors = m_config->processors();

    ::gsim::core::holder srcHolder = m_message->holder();

    for (processors_t::const_iterator it = processors.begin(); 
            it != processors.end(); ++it) 
    {
        applyProcessor(m_message, *it, srcHolder);
    }
    
    // postFunc
    m_evaluator.post(m_message);
    if (m_evaluator.hasError()) throw m_evaluator.error();

    // clone message
    // we can't emit a message we're going to modify

    // Deep copy
    Message_ptr message(m_message->clone());

    // emit message
    emit sendMessage(message);
    m_config->notifyMessageSent(message);
}

void Sender::applyProcessor(
        Message_ptr message,
        MessageProcessor_ptr processor,
        gsim::core::holder holder)
{
    const QList< int >& path = processor->path();

    const TypeDescriptor_ptr op = 
        m_config->messageDescriptor();

    // Results
    TypeDescriptor_ptr descriptor = NULL;
    Holder value;

    bool res = followPath(op, holder, path, 
            // Results
            descriptor, value);

    if (res)
        processor->process(message, value);
}

void Sender::scheduleTimer(Sender_weak weak)
{
    namespace ptime = boost::posix_time;

    m_timer.expires_at(m_timer.expires_at() +
            ptime::milliseconds(m_config->period()));

    m_timer.async_wait(
            boost::bind(&Sender::handleTimeout,
                this, weak, _1));
}

void Sender::handleTimeout(
        Sender_weak weak,
        const boost::system::error_code& err)
{
    if (!err)
    {
        Sender_ptr ptr = weak.lock();
        
        if (ptr)
        {
            try 
            {
                process();

                if (++m_currentTime == m_config->times())
                {
                    m_config->notifyFinished();

                    emit finished(m_config);
                }

                // Last step to ensure thread-safe
                if (m_currentTime != m_config->times())
                    scheduleTimer(weak);
            } 
            catch(const QString& ex)
            {
                m_config->notifyFinished();

                emit finished(m_config);

                emit error(ex);
            }
        }
    }
    else
    {
        // Cancel
    }
}

//
//
// Sender Config
//
//

class SenderConfig::Data
{
public:

    Data(Connection_ptr c,
            TypeDescriptor_ptr md,
            Message_ptr m,
            const QString& co,
            const QList< MessageProcessor_ptr >& ps,
            int ts,
            unsigned int p,
            unsigned int id) :
        connection(c), messageDescriptor(md),
        message(m),
        code(co), processors(ps),
        times(ts), period(p),
        initDelay(id)
    {
    }

    Connection_ptr connection;
    TypeDescriptor_ptr messageDescriptor;
    Message_ptr message;
    const QString code;
    const QList< MessageProcessor_ptr > processors;
    const int times;
    const unsigned int period;
    const unsigned int initDelay;
};

SenderConfig::SenderConfig(
        Connection_ptr connection,
        TypeDescriptor_ptr messageDescriptor,
        Message_ptr message,
        const QString& code,
        const QList< MessageProcessor_ptr >& processors,
        int times,
        unsigned int period, 
        unsigned int initDelay) : 
    m_data(new Data(connection, messageDescriptor, message, code,
                processors, times, period, initDelay))
{
}

SenderConfig::~SenderConfig()
{
    delete m_data;
}

// Accessors
Connection_ptr SenderConfig::connection() const
{
    return m_data->connection;
}

TypeDescriptor_ptr 
SenderConfig::messageDescriptor() const
{
    return m_data->messageDescriptor;
}

Message_ptr SenderConfig::message() const
{
    return m_data->message;
}

const QString& SenderConfig::code() const
{
    return m_data->code;
}

const QList< MessageProcessor_ptr >& 
SenderConfig::processors() const
{
    return m_data->processors;
}

int SenderConfig::times()
{
    return m_data->times;
}

unsigned int SenderConfig::period() const
{
    return m_data->period;
}

void SenderConfig::notifyMessageSent(Message_ptr message)
{
    emit messageSent(message);
}

void SenderConfig::notifyFinished()
{
    emit finished();
}

unsigned int SenderConfig::initDelay() const
{
    return m_data->initDelay;
}

