// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * Application.hpp
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

#ifndef GSIM_QT_APPLICATION_HPP
#define GSIM_QT_APPLICATION_HPP

#include <gsim/qt/types.hpp>

namespace gsim 
{
namespace qt 
{

class MainWindow;

class Application
{
public:

    Application(int& argc, char ** argv);
    ~Application();

    /**
     * @brief Añade una conexión al controlador de la aplicación.
     *
     * @param connection
     */
    void addConnection(Connection_ptr connection);

    /**
     * @brief Muestra la ventana principal e inicia la aplicación.
     *
     * @return El valor devuelvo por el método exec de QApplication.
     */
    int exec();

    /**
     * @brief Devuelve la ventana principal de la aplicación.
     *
     * @return 
     */
    MainWindow * window() const;

    void setTitle(const char * title);

    void setDescription(const char * description);

protected:

    class Data;
    Data * m_data;
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_APPLICATION_HPP */

