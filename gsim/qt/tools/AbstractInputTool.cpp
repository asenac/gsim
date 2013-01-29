// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * AbstractInputTool.cpp
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

#include "AbstractInputTool.hpp"
#include <gsim/qt/model/Connection.hpp>
#include <gsim/qt/widgets/SortableGroup.hpp>
#include <gsim/qt/reflective/path.hpp> // getFieldName
#include <gsim/qt/tools/Engine.hpp>
#include <QHBoxLayout>
#include <QTreeView>

#include <iostream>

using namespace gsim::qt;

// Reflective plot

AbstractInputItem::AbstractInputItem(Connection_ptr connection_,
        TypeDescriptor_ptr message_,
        const QList< int >& path_, 
        QWidget * parent) :
    QWidget(parent), 
    m_connection(connection_), 
    m_message(message_), 
    m_path(path_)
{
}

AbstractInputItem::~AbstractInputItem()
{
}

Connection_ptr AbstractInputItem::connection() const
{
    return m_connection;
}

TypeDescriptor_ptr AbstractInputItem::message() const
{
    return m_message;
}

const QList< int >& AbstractInputItem::path() const
{
    return m_path;
}

MessageProcessor_ptr AbstractInputItem::processor() const
{
    return m_processor;
}

void AbstractInputItem::start()
{
    if (m_processor)
        emit addProcessor(m_processor); 
}

void AbstractInputItem::reset()
{
    if (m_processor)
    {
        emit removeProcessor(m_processor); 
        m_processor.reset();
    }
}

//
//
// Abstract Input Tool
//
//

AbstractInputTool::AbstractInputTool(QWidget * parent) :
    AbstractTool(parent), m_model(NULL)
{
    QHBoxLayout * layout = new QHBoxLayout(this);
    QSplitter * splitter = new QSplitter(Qt::Horizontal, this);

    // Model view
    m_view = new QTreeView(this);
    splitter->addWidget(m_view);
    m_view->setMaximumWidth(300);
    m_view->setColumnWidth(0, 210);

    // Plots
    m_group = new qt::SortableGroup(this);
    m_group->setDelete(false);
    splitter->addWidget(m_group);

    layout->addWidget(splitter);

    setLayout(layout);

    // widget signals
    connect(m_group, 
            SIGNAL(deleteRequested(gsim::qt::SortableGroupItem *)),
            this, 
            SLOT(deleteRequested(gsim::qt::SortableGroupItem *)));
    
    setMinimumSize(650, 400);

    setModel(createModel());
}

AbstractInputTool::~AbstractInputTool()
{
    delete m_model;
}

void AbstractInputTool::setModel(MessageModel * model)
{
    if (m_model)
    {
        delete m_model;
    }

    m_model = model;

    if (model)
    {
        m_view->setModel(m_model);

        // connect model signals 
        connect(m_model, 
                SIGNAL(checked(Connection_ptr,
                        TypeDescriptor_ptr,
                        const QList< int >&)),
                this,
                SLOT(createAbstractInputItem(Connection_ptr,
                        TypeDescriptor_ptr,
                        const QList< int >&)));
        connect(m_model, 
                SIGNAL(unchecked(Connection_ptr,
                        TypeDescriptor_ptr,
                        const QList< int >&)),
                this,
                SLOT(deleteAbstractInputItem(Connection_ptr,
                        TypeDescriptor_ptr,
                        const QList< int >&)));
    }
}

MessageModel * AbstractInputTool::createModel() 
{
    return new MessageModel(MessageModel::kIn, this);
}

void AbstractInputTool::registerInstance(Connection_ptr connection)
{
    m_model->registerInstance(connection);
}

void AbstractInputTool::unregisterInstance(Connection_ptr connection)
{
    // Maps
    map_t::iterator it = m_map.begin();
    for(; it != m_map.end(); it++)
    {
        if (connection.get() == it->first.first)
        {
            for (int i = 0; i < it->second.size(); i++) 
            {
                AbstractInputItem * item = it->second[i];

                m_inverse_map.erase(item);

                // FIXME we need to block signals because uncheck
                // method is connected to deleteAbstractItem
                // Must be changed for a better implementation
                m_model->blockSignals(true);
                m_model->uncheck(connection, item->message(), item->path());
                m_model->blockSignals(false);

                // Notify to the processor
                item->reset();

                m_group->deleteItem(
                        qobject_cast< qt::SortableGroupItem * >
                            (item->parent()));

                delete item;
            }

            m_map.erase(it);
        }
    }

    m_model->unregisterInstance(connection);
}

AbstractInputItem * AbstractInputTool::createAbstractInputItem(
        Connection_ptr connection, 
        TypeDescriptor_ptr message,
        const QList< int >& path)
{
    AbstractInputItem * item = NULL;

    if (message && connection)
    {
        const key_t key(connection.get(), message);
        QList< AbstractInputItem * >& list = m_map[key];

        for (int i = 0; i < list.size(); i++) 
        {
            if (list.at(i)->path() == path)
            {
                return NULL;
            }
        }
        
        item = createItem(connection, message, path);

        list.push_back(item);
        m_inverse_map[item] = key;

        qt::SortableGroupItem * sitem = 
            new qt::SortableGroupItem(item, m_group);

        sitem->showDetails();

        const std::string messageName (
                connection->descriptor()->in()->get_descriptor_name(message));
        const QString title(getFieldName(message, path));
        sitem->setTitle(connection->name() + "." + messageName.c_str() + title);

        m_group->appendItem(sitem);

        // connect with the processor
        QObject * inputMessageProcessorManager = 
            Engine::instance()->inputMessageProcessorManager();
        connect(item, 
                SIGNAL(addProcessor(MessageProcessor_ptr)),
                inputMessageProcessorManager,
                SLOT(addProcessor(MessageProcessor_ptr)));
        connect(item, 
                SIGNAL(removeProcessor(MessageProcessor_ptr)),
                inputMessageProcessorManager,
                SLOT(removeProcessor(MessageProcessor_ptr)));

        item->start();
    }

    return item;
}

void AbstractInputTool::deleteAbstractInputItem(
        Connection_ptr connection,
        TypeDescriptor_ptr message,
        const QList< int >& path)
{
    const key_t key(connection.get(), message);
    QList< AbstractInputItem * >& list = m_map[key];

    for (int i = 0; i < list.size(); i++) 
    {
        AbstractInputItem * item = list[i];
        if (item->path() == path)
        {
            list.removeAt(i);
            m_inverse_map.erase(item);

            // Notify to the processor
            item->reset();

            m_group->deleteItem(
                    qobject_cast< qt::SortableGroupItem * >
                        (item->parent()));

            delete item;
            break;
        }
    }
}

void AbstractInputTool::deleteRequested(qt::SortableGroupItem* item)
{
    AbstractInputItem * aItem = 
        qobject_cast< AbstractInputItem * >(item->getWidget());

    if (aItem)
    {
        inverse_map_t::iterator it = m_inverse_map.find(aItem);

        if (it != m_inverse_map.end())
        {
            const key_t key(it->second);

            m_map[key].removeAll(aItem);

            // notify to model
            m_model->uncheck(aItem->connection(), aItem->message(), aItem->path());

            m_inverse_map.erase(it);
        }
    }

    m_group->deleteItem(item);
}

// 
//
// Load and Save
//
//

void AbstractInputItem::save(QVariant& settings)
{
    QVariantMap map;

    settings = map;
}

void AbstractInputItem::load(const QVariant& settings)
{
    const QVariantMap map = settings.toMap();
}

void AbstractInputTool::save(QVariant& settings)
{
    QVariantList list;

    for (map_t::iterator it = m_map.begin(); 
            it != m_map.end(); ++it) 
    {
        for (int i = 0; i < it->second.size(); i++) 
        {
            QVariantMap map;
            QVariantList vpath;

            const QList< int >& path = it->second.at(i)->path();

            for (int j = 0; j < path.size(); j++) 
            {
                vpath << path.at(j);
            }

            map["connection"] = it->first.first->name();
            map["message"] = getMessageName(it->second.at(i)->message());
            map["path"] = vpath;

            it->second.at(i)->save(map["config"]);

            list << map;
        }
    }

    settings = list;
}

void AbstractInputTool::load(const QVariant& settings)
{
    clear();

    const QVariantList list = settings.toList();

    for (int i = 0; i < list.size(); i++) 
    {
        const QVariantMap map = list.at(i).toMap();

        const QString id = map["connection"].toString();
        const std::string message = map["message"].toString().toStdString();

        const QVariantList vpath = map["path"].toList();
        QList< int > path;
        for (int j = 0; j < vpath.size(); j++) 
        {
            path << vpath.at(j).toInt();
        }

        Connection_ptr connection = m_model->getInstance(id);
        TypeDescriptor_ptr op = NULL;

        if (connection && (op = 
                 connection->descriptor()->in()->get_descriptor_by_name(message.c_str())))

        {
            AbstractInputItem * item = 
                createAbstractInputItem(connection, op, path);

            if (item)
            {
                item->load(map["config"]);

                // Bloqueamos las señales para que no vuelva a crear otro
                m_model->blockSignals(true);
                m_model->check(connection, op, path);
                m_model->blockSignals(false);
            }
        }
    }
}

void AbstractInputTool::clear()
{
    for (inverse_map_t::const_iterator it = m_inverse_map.begin(); 
            it != m_inverse_map.end(); ++it) 
    {
        AbstractInputItem * item = it->first;

        // Notify to the processor
        item->reset();

        m_group->deleteItem(
                qobject_cast< qt::SortableGroupItem * >
                    (item->parent()));

        delete item;
    }

    m_map.clear();
    m_inverse_map.clear();
}

