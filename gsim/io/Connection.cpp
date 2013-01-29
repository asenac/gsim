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

using namespace gsim::io;

Connection::Connection() : 
    m_work(m_io_service), 
    m_thread(boost::bind(&boost::asio::io_service::run, 
                &m_io_service)),
    m_status(kDisconnected)
{
}

Connection::~Connection() 
{
    stop();
}

boost::asio::io_service& Connection::get_io_service() 
{
    return m_io_service;
}

void Connection::join() 
{
    m_thread.join();
}

void Connection::set_callback(
        const Connection::callback_t& callback) 
{
    m_callback = callback;
}

void Connection::set_status_callback(
        const Connection::status_callback_t& callback) 
{
    m_status_callback = callback;
}

void Connection::set_error_callback(
        const Connection::error_callback_t& callback) 
{
    m_error_callback = callback;
}

void Connection::stop() 
{
    m_io_service.stop();
    join();
}

void Connection::produce_error(const std::string& error)
{
    if (m_error_callback)
    {
        m_io_service.post(
                boost::bind(m_error_callback, error));
    }
}

void Connection::set_status(Status status)
{
    m_status = status;

    if (m_status_callback)
    {
        m_status_callback(status);
    }
}

