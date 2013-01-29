// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * initialize.cpp
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

#include "initialize.hpp"
#include <gsim/core/properties.hpp> // initializer
#include <gsim/qt/model/Connection.hpp>
#include <gsim/qt/tools/Engine.hpp>
#include <gsim/qt/tools/Sender.hpp>
#include <gsim/qt/proc/MessageProcessor.hpp>

namespace  
{

    void __initialize()
    {
        // Qt
        {
            using namespace gsim::qt;

            qRegisterMetaType< Message_ptr >
                ("Message_ptr");
            qRegisterMetaType< Connection_ptr >
                ("Connection_ptr");
            qRegisterMetaType< ConnectionConfig_ptr >
                ("ConnectionConfig_ptr");
            qRegisterMetaType< ConnectionStatus >
                ("ConnectionStatus");
            qRegisterMetaType< MessageProcessor_ptr >
                ("MessageProcessor_ptr");
            qRegisterMetaType< Holder >
                ("Holder");
            qRegisterMetaType< SenderConfig_ptr >
                ("SenderConfig_ptr");

            // Inicia el motor de las herramientas
            Engine::instance();
        }
    }

} // namespace 

namespace gsim 
{
namespace qt 
{

    void initialize()
    {
        static core::initializer _instance(&__initialize);
    }

} // namespace qt
} // namespace gsim

