// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * MessageProcessor.cpp
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

#include "MessageProcessorManager.hpp"
#include <gsim/qt/reflective/path.hpp> // followPath
#include <gsim/core/event.hpp>

using namespace gsim::qt;

MessageProcessorManager::MessageProcessorManager(QObject * parent) :
    QObject(parent)
{
}

MessageProcessorManager::~MessageProcessorManager()
{
}

void MessageProcessorManager::processMessage(
        Connection_ptr con,
        Message_ptr msg)
{
    // Obtiene el contenido y el descriptor del mensaje
    Holder holder = msg->holder();
    TypeDescriptor_ptr messageDescriptor = 
        holder.get_type_descriptor();

    // creates the key
    map_t::iterator it = m_processors.find(
            std::make_pair(con.get(), messageDescriptor));

    if (it != m_processors.end() && 
            !it->second.empty())
    {
        // Iterates over its associated processors
        processors_t::const_iterator pit = it->second.begin(); 

        for (; pit != it->second.end(); ++pit) 
        {
            const MessageProcessor_ptr processor = *pit;
            const ReflectivePath_t& path = processor->path();

            // Results
            TypeDescriptor_ptr descriptor = NULL;
            Holder value;

            bool res = followPath(messageDescriptor, holder, path, 
                    // Results
                    descriptor, value);

            if (res)
                processor->process(msg, value);
        }
    }
}

void MessageProcessorManager::addProcessor(
        MessageProcessor_ptr p)
{
    TypeDescriptor_ptr message = p->messageDescriptor();

    // Inserts the processor
    const key_t key (p->connection().get(), message);
    m_processors[key].push_back(p);
    
    emit addedProcessor(p);
}

void MessageProcessorManager::removeProcessor(
        MessageProcessor_ptr p)
{
    const TypeDescriptor_ptr message = p->messageDescriptor();

    // Removes the processor
    const key_t key (p->connection().get(), message);
    m_processors[key].removeAll(p);

    emit removedProcessor(p);
}

