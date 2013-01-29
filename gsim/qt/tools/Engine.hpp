// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * Engine.hpp
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

#ifndef GSIM_QT_ENGINE_HPP
#define GSIM_QT_ENGINE_HPP

#include <QtCore>

namespace gsim 
{
namespace qt 
{

class Engine : public QObject
{
    Q_OBJECT
protected:
    Engine();

public:
    ~Engine();

    static Engine * instance();

    QObject * inputMessageProcessorManager() const;

    QObject * outputMessageProcessorManager() const;
    
    QObject * senderController() const;

signals:

    void message(const QString&);
    void error(const QString&);

protected:
    class Data;
    Data * m_data;
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_ENGINE_HPP */

