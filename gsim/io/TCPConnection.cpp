// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * TCPConnection.cpp
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

#include "TCPConnection.hpp"

using namespace gsim::io;

TCPConnection::TCPConnection() : 
    m_acceptor(get_io_service(), 
            boost::asio::ip::tcp(boost::asio::ip::tcp::v4())), 
    m_socket(get_io_service(), 
            boost::asio::ip::tcp(boost::asio::ip::tcp::v4())) 
{
    // TODO establecerlo por parámetro
    boost::asio::socket_base::receive_buffer_size 
        option(65535 * 2);
    m_socket.set_option(option);
}

TCPConnection::~TCPConnection() 
{
}

void TCPConnection::listen(const char * local_host, 
        unsigned short local_port) 
{
    namespace ip = boost::asio::ip;
    
    // Liga el socket con el puerto local...
    ip::tcp::endpoint local_endpoint(
       ip::address::from_string(local_host), local_port);
    m_acceptor.bind(local_endpoint);

    set_status(Connection::kListening);

    m_acceptor.async_accept(m_socket, 
            boost::bind(&TCPConnection::handle_connect,
                this, _1));
}

void TCPConnection::connect(const char * remote_host, 
        unsigned short remote_port) 
{
    namespace ip = boost::asio::ip;
    
    // Liga el socket con el puerto remoto
    ip::tcp::endpoint remote_endpoint(
       ip::address::from_string(remote_host), remote_port);

    set_status(Connection::kConnecting);

    m_socket.async_connect(remote_endpoint,
            boost::bind(&TCPConnection::handle_connect,
                this, _1)); 
}

boost::asio::ip::tcp::socket& TCPConnection::get_socket() 
{
    return m_socket;
}

void TCPConnection::start_read() 
{
    new_read();
}

void TCPConnection::send(const char * buffer, 
        std::size_t size) 
{
    namespace as = boost::asio;
    
    as::const_buffers_1 buf(buffer, size);
    
    try 
    {
    	m_socket.send(buf);
    }
    catch(...) 
    {
        produce_error("Error sending");
    }
}

void TCPConnection::handle_read(const boost::system::error_code& error, 
        size_t bytes_transferred) 
{
    namespace as = boost::asio;
    
    if (!error)
    {
    	m_in_buffer.commit(bytes_transferred);
    	
    	if (m_callback)
    	{
    		const char * buffer = 
    			as::buffer_cast< const char * >(
                        m_in_buffer.data());
    		
    		// Despues del callback los datos dejan de 
            // estar disponibles	
    		std::size_t can_consume = 
                m_callback(buffer, m_in_buffer.size());
    		
    		m_in_buffer.consume(can_consume);
    	}
    	else
    	{
            // Puede no haber callback
    		m_in_buffer.consume(bytes_transferred);
    	}
    }
    else
    {
        produce_error("Error receiving");
    }
    
    // programar nueva recepcion solo si no ha sido cancelada
    if (error != boost::system::errc::operation_canceled)
    {
    	new_read();
    }
}

void TCPConnection::new_read() 
{
    namespace as = boost::asio;
    
    // previously binded
    // TODO establecer MAX_PACKET_SIZE
    m_socket.async_receive(m_in_buffer.prepare(1024 * 1024), 
    	boost::bind(&TCPConnection::handle_read, this, 
    	as::placeholders::error,
    	as::placeholders::bytes_transferred));
}

void TCPConnection::handle_accept(
        const boost::system::error_code& error)
{
    if (!error)
    {
        set_status(Connection::kConnected);
    }
    else
    {
        produce_error("Error connecting");
    }
}

void TCPConnection::handle_connect(
        const boost::system::error_code& error)
{
    if (!error)
    {
        set_status(Connection::kConnected);
    }
    else
    {
        produce_error("Error connecting");
    }
}

