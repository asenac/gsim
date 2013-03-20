// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * GenericConnection.hpp
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

#ifndef GSIM_QT_GENERICCONNECTION_HPP
#define GSIM_QT_GENERICCONNECTION_HPP

#include <gsim/qt/model/Connection.hpp>

namespace gsim 
{
namespace qt 
{

class GenericConnection : public Connection
{
    Q_OBJECT
public:

    GenericConnection(QObject * parent);
    GenericConnection(
            ConnectionDescriptor_ptr descriptor = ConnectionDescriptor_ptr(),
            QObject * parent = 0);
    virtual ~GenericConnection();

    /**
     * @brief Reimplementar para procesar los datos recibidos por la 
     *        conexión. 
     *
     * @param data Los datos recibidos.
     * @param size El tamaño de los datos recibidos.
     *
     * @return Ha de devolver el número de bytes que se pueden consumir.
     *         Los que no se consuman serán vueltos a notificar cuando
     *         llegen más datos. 
     */
    virtual std::size_t processData(const char * data, std::size_t size);

    /**
     * @brief Utilizar este método para enviar.
     *
     * @param data Los datos a enviar.
     * @param size El tamaño de los datos a enviar.
     */
    virtual void send(const char * data, std::size_t size);

    virtual bool applyConfig(ConnectionConfig_ptr cfg);

protected slots:

    void readPendingData();

protected:

    class Data;
    Data * m_data;
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_GENERICCONNECTION_HPP */

