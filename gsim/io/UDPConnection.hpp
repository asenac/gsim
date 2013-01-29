// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * UDPConnection.hpp
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

#ifndef GSIM_IO_UDPCONNECTION_HPP
#define GSIM_IO_UDPCONNECTION_HPP

#include <gsim/io/Connection.hpp>

namespace gsim 
{
namespace io 
{

class UDPConnection : public Connection 
{
public:
    
    UDPConnection();
    
    ~UDPConnection();
    
    void bind(const char * local_host, unsigned short local_port);
    
    void bind_and_connect(const char * local_host, 
            unsigned short local_port, 
            const char * remote_host, 
            unsigned short remote_port);
    
    void connect(const char * remote_host, unsigned short remote_port);
    
    boost::asio::ip::udp::socket& get_socket();
    
    void send(const char * buffer, std::size_t size);
    
    void start_read();
    
protected:

    void handle_read(const boost::system::error_code& error, 
            size_t bytes_transferred);
    
    void new_read();
    
    boost::asio::ip::udp::socket m_socket;    
};

} // namespace io
} // namespace gsim

#endif /* GSIM_IO_UDPCONNECTION_HPP */

