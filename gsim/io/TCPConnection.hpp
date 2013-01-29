// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * TCPConnection.hpp
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

#ifndef GSIM_IO_TCPCONNECTION_HPP
#define GSIM_IO_TCPCONNECTION_HPP

#include <gsim/io/Connection.hpp>

namespace gsim 
{
namespace io 
{

class TCPConnection : public Connection 
{
public:
    
    TCPConnection();
    
    ~TCPConnection();
    
    void listen(const char * local_host, 
            unsigned short local_port);
    
    void connect(const char * remote_host, 
            unsigned short remote_port);
    
    boost::asio::ip::tcp::socket& get_socket();
    
    void send(const char * buffer, std::size_t size);
    
    void start_read();
    
protected:

    void handle_read(const boost::system::error_code& error, 
            size_t bytes_transferred);

    void handle_accept(
            const boost::system::error_code& error);
    
    void handle_connect(
            const boost::system::error_code& error);
    
    void new_read();
    
    boost::asio::ip::tcp::acceptor m_acceptor;
    boost::asio::ip::tcp::socket m_socket;
};

} // namespace io
} // namespace gsim

#endif /* GSIM_IO_TCPCONNECTION_HPP */

