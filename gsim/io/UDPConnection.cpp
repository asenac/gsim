// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * UDPConnection.cpp
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

#include "UDPConnection.hpp"

using namespace gsim::io;

UDPConnection::UDPConnection() : 
    m_socket(get_io_service(), 
            boost::asio::ip::udp(boost::asio::ip::udp::v4())) 
{
    // TODO establecerlo por parámetro
    boost::asio::socket_base::receive_buffer_size 
        option(65535 * 2);
    m_socket.set_option(option);
}

UDPConnection::~UDPConnection() 
{
}

void UDPConnection::bind(const char * local_host, 
        unsigned short local_port) 
{
    namespace ip = boost::asio::ip;
    
    // Liga el socket con el puerto local...
    ip::udp::endpoint local_endpoint(
       ip::address::from_string(local_host), local_port);
    m_socket.bind(local_endpoint);
}

void UDPConnection::bind_and_connect(const char * local_host, 
        unsigned short local_port, 
        const char * remote_host, 
        unsigned short remote_port) 
{
    namespace ip = boost::asio::ip;
    
    // Liga el socket con el puerto local...
    bind(local_host, local_port);

    // ...y con el remoto
    connect(remote_host, remote_port);
}

void UDPConnection::connect(const char * remote_host, 
        unsigned short remote_port) 
{
    namespace ip = boost::asio::ip;
    
    // Liga el socket con el puerto remoto
    ip::udp::endpoint remote_endpoint(
       ip::address::from_string(remote_host), remote_port);
    m_socket.connect(remote_endpoint); 
}

boost::asio::ip::udp::socket& UDPConnection::get_socket() 
{
    return m_socket;
}

void UDPConnection::start_read() 
{
    new_read();
}

void UDPConnection::send(const char * buffer, 
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

void UDPConnection::handle_read(
        const boost::system::error_code& error, 
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

void UDPConnection::new_read() 
{
    namespace as = boost::asio;
    
    // previously binded
    // TODO establecer MAX_PACKET_SIZE
    m_socket.async_receive(m_in_buffer.prepare(1024 * 1024), 
    	boost::bind(&UDPConnection::handle_read, this, 
    	as::placeholders::error,
    	as::placeholders::bytes_transferred));
}

