// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * Sender.hpp
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

#ifndef GSIM_QT_SENDER_HPP
#define GSIM_QT_SENDER_HPP

#include <QtCore>
#include <QtScript>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <gsim/qt/types.hpp>
#include <gsim/qt/export.hpp>
#include <gsim/qt/proc/MessageProcessor.hpp>
#include <gsim/qt/script/ScriptEvaluator.hpp>

namespace gsim 
{
namespace qt 
{

class SenderConfig;

typedef boost::shared_ptr< SenderConfig > SenderConfig_ptr;

class Sender;

typedef boost::shared_ptr< Sender > Sender_ptr;
typedef boost::weak_ptr< Sender > Sender_weak;

class SenderController;

class GSIM_QT_DECLSPEC SenderConfig : public QObject
{
    Q_OBJECT
public:

    SenderConfig(
        Connection_ptr connection,
        TypeDescriptor_ptr messageDescriptor,
        Message_ptr message,
        const QString& code,
        const QList< MessageProcessor_ptr >& processors,
        int times,
        unsigned int period,
        unsigned int initDelay = 0);
    ~SenderConfig();

    // Accessors
    Connection_ptr connection() const;
    TypeDescriptor_ptr messageDescriptor() const;
    Message_ptr message() const;
    const QString& code() const;
    const QList< MessageProcessor_ptr >& processors() const;
    int times();
    unsigned int period() const;
    unsigned int initDelay() const;

public slots:

    void notifyMessageSent(Message_ptr);
    void notifyFinished();

signals:

    void messageSent(Message_ptr);
    void finished();

protected:

    class Data;
    Data * m_data;
};

class GSIM_QT_DECLSPEC Sender : public QObject
{
    Q_OBJECT
public:

    Sender(boost::asio::io_service& ioService,
        SenderConfig_ptr config);
    ~Sender();

    void start(Sender_weak weak);

    void cancel();

    void process();

signals:

    void finished(SenderConfig_ptr);
    void sendMessage(Message_ptr);
    void error(const QString& err);

protected:

    void scheduleTimer(
            Sender_weak weak);

    void handleTimeout(
            Sender_weak weak,
            const boost::system::error_code& error);

    void applyProcessor(
            Message_ptr message,
            MessageProcessor_ptr processor,
            Holder holder);

    boost::asio::deadline_timer m_timer;
    SenderConfig_ptr m_config;
    int m_currentTime;

    MessageEvaluator m_evaluator;
    Message_ptr m_message;
};

class GSIM_QT_DECLSPEC SenderController : public QObject
{
    Q_OBJECT
public:

    SenderController();
    ~SenderController();

    void start(unsigned int numberOfThreads = 5);
    void join();
    void stop();

public slots:

    void addSender(SenderConfig_ptr cfg);
    void deleteSender(SenderConfig_ptr cfg);

signals:

    void error(const QString& err);

protected:

    boost::thread_group m_threads;
    boost::asio::io_service m_ioService;
    boost::asio::io_service::work m_work;

    typedef QMap< SenderConfig_ptr, Sender_ptr > map_t;
    map_t m_map;
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_SENDER_HPP */

