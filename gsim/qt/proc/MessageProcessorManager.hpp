// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * MessageProcessorManager.hpp
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

#ifndef GSIM_QT_MESSAGEPROCESSORMANAGER_HPP
#define GSIM_QT_MESSAGEPROCESSORMANAGER_HPP

#include <map>
#include <QObject>
#include <gsim/qt/types.hpp>
#include <gsim/qt/export.hpp>
#include <gsim/qt/proc/MessageProcessor.hpp>

namespace gsim 
{
namespace qt 
{

/**
 * @brief Process input requests in a non-GUI thread.
 *
 * Use moveToThread to process its events in other thread.
 */
class GSIM_QT_DECLSPEC MessageProcessorManager : 
    public QObject
{
    Q_OBJECT
public:

    MessageProcessorManager(QObject * parent = 0);
    virtual ~MessageProcessorManager();

public slots:

    void processMessage(Connection_ptr con, Message_ptr msg);

    // Processors
    void addProcessor(MessageProcessor_ptr);
    void removeProcessor(MessageProcessor_ptr);

signals:

    void addedProcessor(MessageProcessor_ptr);
    void removedProcessor(MessageProcessor_ptr);

protected:

    typedef std::pair< Connection *, TypeDescriptor_ptr > key_t;
    typedef QList< MessageProcessor_ptr > processors_t;
    typedef std::map< key_t, processors_t > map_t;

    /**
     * @brief Registred processors.
     *
     * Must be optimized using a tree-based collection.
     */
    map_t m_processors;

};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_MESSAGEPROCESSORMANAGER_HPP */

