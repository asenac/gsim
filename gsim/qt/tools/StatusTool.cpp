// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * StatusTool.cpp
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

#include "StatusTool.hpp"
#include <gsim/qt/model/Connection.hpp>
#include <gsim/qt/model/StatusManager.hpp>
#include <gsim/qt/reflective/ReflectiveWidget.hpp>
#include <gsim/qt/reflective/WidgetFactory.hpp>
#include <gsim/qt/reflective/DragAndDrop.hpp>
#include <gsim/core/descriptor_utils.hpp>

//
// StatusTool
//

using namespace gsim::qt;

class StatusTool::Data
{
public:

    Data(Connection_ptr c) : 
        connection(c),
        current(items.end())
    {
    }

    Connection_ptr connection;

    typedef std::map< QString, StatusItem * > items_t;
    items_t items;

    items_t::iterator current;

    QStringListModel * model;

    QStackedWidget * proposedStack;
    QStackedWidget * currentStack;
};

StatusTool::StatusTool(Connection_ptr connection, QWidget * parent) :
    QWidget(parent), m_data(new Data(connection))
{
    StatusManager * statusManager = connection->statusManager();

    assert(statusManager);

    QVBoxLayout * layout = new QVBoxLayout;
    QSplitter * splitter = new QSplitter(Qt::Horizontal, this);

    // Status view
    QListView * view = new QListView(this);
    m_data->model = new QStringListModel(this);
    m_data->model->setStringList(statusManager->statusList());
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view->setModel(m_data->model);
    splitter->addWidget(view);

    connect(view, 
            SIGNAL(clicked(const QModelIndex&)),
            this,
            SLOT(selectedStatus(const QModelIndex&)));

    QWidget * mainWidget = new QWidget(this);
    {
        QGridLayout * layout = new QGridLayout(mainWidget);

        // proposed
        layout->addWidget(new QLabel("<b>Proposed status</b>"), 1, 0);
        m_data->proposedStack = new QStackedWidget();
        layout->addWidget(m_data->proposedStack, 2, 0);

        // current
        layout->addWidget(new QLabel("<b>Current status</b>"), 1, 1);
        m_data->currentStack = new QStackedWidget();
        layout->addWidget(m_data->currentStack, 2, 1);

        // Proposed buttons
        QDialogButtonBox * box = new QDialogButtonBox(mainWidget);
        QPushButton * applyBtn = box->addButton(QDialogButtonBox::Apply);
        QPushButton * copyBtn = box->addButton("&Copy current",
                QDialogButtonBox::ActionRole);
        QPushButton * clearBtn = box->addButton("&Clear",
                QDialogButtonBox::ActionRole);
        layout->addWidget(box, 3, 0);

        connect(applyBtn, SIGNAL(clicked()), this, SLOT(apply()));
        connect(copyBtn, SIGNAL(clicked()), this, SLOT(copyCurrent()));
        connect(clearBtn, SIGNAL(clicked()), this, SLOT(clearProposed()));

        // Current buttons
        box = new QDialogButtonBox(mainWidget);
        QPushButton * sendBtn = box->addButton("&Send current",
                QDialogButtonBox::ActionRole);
        layout->addWidget(box, 3, 1);

        connect(sendBtn, SIGNAL(clicked()), this, SLOT(sendCurrent()));

        mainWidget->setLayout(layout);
    }
    splitter->addWidget(mainWidget);

    layout->addWidget(splitter);
    setLayout(layout);

    connect(this, SIGNAL(updateStatus(const QString&, Holder)),
            statusManager, 
            SLOT(setStatus(const QString&, Holder)));
    connect(this, SIGNAL(sendCurrent(const QString&)),
            connection.get(), 
            SLOT(sendStatus(const QString&)));
    connect(statusManager, 
            SIGNAL(statusChanged(const QString&, Holder)),
            this,
            SLOT(setCurrentStatus(const QString&, Holder)));

    setWindowTitle(QString("Status manager for ") + connection->name());
}

StatusTool::~StatusTool()
{
    delete m_data->model;
    delete m_data;
}

void StatusTool::setCurrentStatus(const QString& name, Holder currentStatus)
{
    // Si es el mostrado actualmente se actualiza;
    // si no lo es se actualizará cuando se muestre
    if (m_data->current != m_data->items.end() && 
            m_data->current->first == name)
    {
        m_data->current->second->setCurrentStatus(currentStatus);
    }
}

void StatusTool::selectedStatus(const QModelIndex& idx)
{
    if (!idx.isValid())
        return;

    const QString name (m_data->model->stringList().at(idx.row()));

    Data::items_t::iterator it = m_data->items.find(name);

    if (it != m_data->items.end())
    {
        m_data->current = it;

        m_data->current->second->setCurrentStatus(
                m_data->connection->statusManager()->status(name));
    }
    else
    {
        Holder current = m_data->connection->statusManager()->status(name);
        StatusItem * item = new StatusItem(name, current, this);

        std::pair< Data::items_t::iterator, bool > res = 
            m_data->items.insert(std::make_pair(name, item));

        m_data->current = res.first;

        m_data->currentStack->addWidget(item->currentWidget());
        m_data->proposedStack->addWidget(item->proposedWidget());
    }

    // Muestra los widgets del mensaje seleccionado
    m_data->currentStack->setCurrentWidget(
            m_data->current->second->currentWidget());
    m_data->proposedStack->setCurrentWidget(
            m_data->current->second->proposedWidget());
}

void StatusTool::apply()
{
    if (m_data->current != m_data->items.end())
    {
        Holder proposed (m_data->current->second->proposedStatus());

        emit updateStatus(m_data->current->first, proposed);
    }
}

void StatusTool::copyCurrent()
{
    if (m_data->current != m_data->items.end())
    {
        m_data->current->second->copyCurrent();
    }
}

void StatusTool::sendCurrent()
{
    if (m_data->current != m_data->items.end())
    {
        emit sendCurrent(m_data->current->first);
    }
}

void StatusTool::clearProposed()
{
    if (m_data->current != m_data->items.end())
    {
        m_data->current->second->clearProposed();
    }
}

//
// StatusItem
//

class StatusItem::Data
{
public:

    Data(const QString& n, TypeDescriptor_ptr m) : 
        descriptor(m),
        name(n),

        proposedWidget(NULL),
        currentWidget(NULL),

        proposedScroll(NULL),
        currentScroll(NULL),

        proposed(NULL),
        current(NULL),

        dragProposed(NULL),
        dropProposed(NULL),
        dragCurrent(NULL)
    {
    }

    TypeDescriptor_ptr descriptor;
    const QString name;
    
    QWidget * proposedWidget;
    QWidget * currentWidget;

    QScrollArea * proposedScroll;
    QScrollArea * currentScroll;

    ReflectiveWidget * proposed;
    ReflectiveWidget * current;

    Drag * dragProposed;
    Drop * dropProposed;
    Drag * dragCurrent;
};

StatusItem::StatusItem(const QString& name, 
        Holder currentStatus, 
        QWidget * parent) :
    QObject(parent), 
    m_data(new Data(name, currentStatus.get_type_descriptor()))
{
    m_data->proposedWidget = 
        WidgetFactoryRepository::create(m_data->descriptor, parent);
    m_data->currentWidget = 
        WidgetFactoryRepository::create(m_data->descriptor, parent);

    m_data->proposedScroll = new QScrollArea(parent);
    m_data->proposedScroll->setWidget(m_data->proposedWidget);
    m_data->proposedScroll->setWidgetResizable(true);

    m_data->currentScroll = new QScrollArea(parent);
    m_data->currentScroll->setWidget(m_data->currentWidget);
    m_data->currentScroll->setWidgetResizable(true);

    m_data->proposed = 
        dynamic_cast< ReflectiveWidget * >(m_data->proposedWidget);
    m_data->current = 
        dynamic_cast< ReflectiveWidget * >(m_data->currentWidget);

    // El actual se muestra en modo de solo lectura
    // Cambiará por actualizaciones notificadas desde la 
    // conexión
    if (m_data->current)
    {
        m_data->current->_setReadOnly(true);
        m_data->dragCurrent = new Drag(m_data->currentWidget, this);
    }

    if (m_data->proposed)
    {
        m_data->dragProposed = new Drag(m_data->proposedWidget, this);
        m_data->dropProposed = new Drop(m_data->proposedWidget, this);
    }

    setCurrentStatus(currentStatus);
}

StatusItem::~StatusItem()
{
    delete m_data;
}

QWidget * StatusItem::currentWidget() const
{
    return m_data->currentScroll;
}

QWidget * StatusItem::proposedWidget() const
{
    return m_data->proposedScroll;
}

void StatusItem::setCurrentStatus(Holder currentStatus)
{
    // Copia el contenido al widget
    if (currentStatus.is_valid() && m_data->current)
    {
        m_data->current->fromHolder(currentStatus);
    }
}

Holder StatusItem::proposedStatus()
{
    Holder proposed (m_data->descriptor->create_holder());

    if (m_data->proposed)
    {
        m_data->proposed->toHolder(proposed);
    }

    return proposed;
}

void StatusItem::copyCurrent()
{
    if (m_data->current && m_data->proposed)
    {
        Holder h (m_data->descriptor->create_holder());
        m_data->current->toHolder(h);
        m_data->proposed->fromHolder(h);
    }
}

void StatusItem::clearProposed()
{
    if (m_data->current && m_data->proposed)
    {
        Holder h ( ::gsim::core::utils::create_initialized_holder(
                    m_data->descriptor));
        m_data->proposed->fromHolder(h);
    }
}

