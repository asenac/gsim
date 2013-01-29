// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * Connection.hpp
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

#ifndef GSIM_IO_CONNECTION_HPP
#define GSIM_IO_CONNECTION_HPP

#include <string>
#include <boost/function.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

namespace gsim 
{
namespace io 
{

class Connection;

typedef boost::shared_ptr< Connection > Connection_ptr;

class Connection
{
public:

    enum Status
    {
        kDisconnected,
        kListening,
        kConnecting,
        kConnected,
        kError
    };

    typedef boost::function< std::size_t (
            const char * buffer, std::size_t size) > 
        callback_t;
    typedef boost::function< void (Status) > 
        status_callback_t;
    typedef boost::function< void (const std::string&) > 
        error_callback_t;

    Connection();
    virtual ~Connection();

    virtual void start_read() = 0;

    virtual void send(const char * buffer, 
            std::size_t size) = 0;

    void set_callback(const callback_t& callback);

    void set_status_callback(
            const status_callback_t& callback);

    void set_error_callback(const error_callback_t& callback);

    virtual void stop();

    void join();

    boost::asio::io_service& get_io_service();

protected:

    void produce_error(const std::string& error);

    void set_status(Status status);

    boost::asio::io_service m_io_service;
    boost::asio::io_service::work m_work;    
    boost::thread m_thread;    
    boost::asio::streambuf m_in_buffer;    

    callback_t m_callback;
    status_callback_t m_status_callback;
    error_callback_t m_error_callback;

    Status m_status;
};

} // namespace io
} // namespace gsim

#endif /* GSIM_IO_CONNECTION_HPP */

