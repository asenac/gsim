// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * ScriptEngine.hpp
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

#ifndef GSIM_QT_SCRIPTENGINE_HPP
#define GSIM_QT_SCRIPTENGINE_HPP

#include <QtScript>
#include <gsim/qt/types.hpp>

namespace gsim 
{
namespace qt 
{

class ScriptEngine : public QObject
{
    Q_OBJECT
public:

    ScriptEngine(QObject * parent = 0);
    ~ScriptEngine();

public slots:

    void run(const QString& code);

    void connectionCreated(Connection_ptr);
    void connectionDeleted(Connection_ptr);

    void messageSent(Connection_ptr, Message_ptr);
    void messageReceived(Connection_ptr, Message_ptr);

signals:

    void error(const QString&);

protected:

    class Data;
    Data * m_data;

    friend class Data;
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_SCRIPTENGINE_HPP */

