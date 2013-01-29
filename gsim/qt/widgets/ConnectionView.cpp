// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * ConnectionView.cpp
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

#include "ConnectionView.hpp"
#include <vector>
#include <QMenu>
#include <QDockWidget>
#include <gsim/core/descriptor.hpp>
#include <gsim/qt/widgets/ConnectionWidget.hpp>

#include <gsim/qt/reflective/SendMessageDialog.hpp>
#include <gsim/qt/reflective/MessageForm.hpp>

#include <gsim/qt/tools/StatusTool.hpp>
#include <gsim/qt/model/StatusManager.hpp>

using namespace gsim::qt;

struct ConnectionView::Data
{

    Data(Connection_ptr con_) :
        connection(con_), 
        menu(new QMenu(con_->name())),
        statusWidget(new ::gsim::qt::ConnectionStatusWidget(con_)),
        statusTool(NULL)
    {
    }

    ~Data()
    {
        delete menu;
        delete statusWidget;

        // Delete dialogs
        for (unsigned int i = 0; i < dialogs.size(); i++) 
        {
            delete dialogs[i];
            delete subwindows[i];
        }

        delete statusTool;
    }

    Connection_ptr connection;
    QMenu * menu;
    QWidget * statusWidget;

    QSignalMapper * senderDialogMapper;

    std::vector< QWidget * > dialogs;
    std::vector< QMdiSubWindow * > subwindows;

    ::gsim::qt::ConnectionWidget * connectionWidget; 
    QDockWidget * connectionDockWidget;

    StatusTool * statusTool;
};

ConnectionView::ConnectionView(Connection_ptr con,
        QObject * parent) :
    QObject(parent), m_data(new Data(con))
{
    using namespace gsim::qt;

    // Connection Widget
    {
        ConnectionWidgetFactory * factory = 
            ConnectionWidgetFactory::instance();

        // Crea un widget asociado con su tipo
        m_data->connectionWidget = 
            factory->createWidget(con->type(), NULL);
        m_data->connectionDockWidget = new QDockWidget(
                QString("%1 configuration").arg(con->name()));
        m_data->connectionDockWidget->setWidget(m_data->connectionWidget);

        // Conecta la señal que configurará la conexión
        connect(m_data->connectionWidget, 
                SIGNAL(applyConfig(ConnectionConfig_ptr)),
                con.get(), 
                SLOT(setConfig(ConnectionConfig_ptr)));

        connect(con.get(), 
                SIGNAL(statusChanged(ConnectionStatus)),
                m_data->connectionWidget,
                SLOT(statusChanged(ConnectionStatus)));
        connect(con.get(), 
                SIGNAL(configChanged(ConnectionConfig_ptr)),
                m_data->connectionWidget,
                SLOT(configChanged(ConnectionConfig_ptr)));

        // Inicializa el panel
        m_data->connectionWidget->configChanged(con->config());
    }

    m_data->senderDialogMapper = new QSignalMapper(this);
    connect(m_data->senderDialogMapper, SIGNAL(mapped(int)),
            this, SLOT(showSenderDialog(int)));

    m_data->menu->addAction("Configuration",
            this, SLOT(showConfig()));

    QMenu * sendMenu = m_data->menu->addMenu("Send messages");

    const ::gsim::core::descriptor_group_ptr out_ =
        con->descriptor()->out();

    unsigned int count = out_->get_descriptor_count();
    m_data->dialogs.resize(count, NULL);
    m_data->subwindows.resize(count, NULL);

    for (unsigned int i = 0; i < count; i++) 
    {
        QAction * act = 
            sendMenu->addAction(out_->get_descriptor_name(i).c_str(),
                    m_data->senderDialogMapper, SLOT(map()));

        m_data->senderDialogMapper->setMapping(act, (int) i);
    }

    /*
    const ::gsim::core::descriptor_group_ptr in_ =
        con->descriptor()->in();
     */

    // Status Tool
    StatusManager * statusManager = con->statusManager();
    if (statusManager)
    {
        m_data->menu->addSeparator();

        QMainWindow * mainWindow = qobject_cast< QMainWindow * >(parent);
        QMdiArea * mdiArea = 
            qobject_cast< QMdiArea * >(mainWindow->centralWidget());

        QMdiSubWindow * subWindow = new QMdiSubWindow(mdiArea);
        m_data->statusTool = 
            new StatusTool(con, static_cast< QWidget * >(parent));
        subWindow->setWidget(m_data->statusTool);

        QAction * action = 
            m_data->menu->addAction("Status", subWindow, SLOT(showNormal()));
        connect(action, SIGNAL(triggered()), subWindow, SLOT(raise()));
        connect(action, SIGNAL(triggered()), m_data->statusTool, SLOT(show()));

        m_data->menu->addSeparator();

        // Save and load
        m_data->menu->addAction("S&ave status", this, SLOT(saveStatus()));
        m_data->menu->addAction("&Load status", this, SLOT(loadStatus()));
    }
}

ConnectionView::~ConnectionView()
{
    delete m_data;
}

Connection_ptr ConnectionView::connection() const
{
    return m_data->connection;
}

QMenu * ConnectionView::menu() const
{
    return m_data->menu;
}

QWidget * ConnectionView::statusWidget() const
{
    return m_data->statusWidget;
}

QWidget * ConnectionView::connectionWidget() const
{
    return m_data->connectionWidget;
}

QDockWidget * ConnectionView::connectionDockWidget() const
{
    return m_data->connectionDockWidget;
}

void ConnectionView::showSenderDialog(int idx)
{
    if (!m_data->dialogs[idx])
    {
        ::gsim::core::descriptor_ptr descriptor = 
            m_data->connection->descriptor()->out(
                    )->get_descriptor_by_index(idx);

        const QString name = getMessageName(descriptor);

#if 0
        m_data->dialogs[idx] = 
            new SendMessageDialog(name, descriptor, 
                    qobject_cast< QWidget * >(parent()));

        connect(m_data->dialogs[idx], SIGNAL(sendMessage(Message_ptr)),
                m_data->connection.get(), SLOT(sendMessage(Message_ptr)));
#else
        MessageSender * sender = new MessageSender(m_data->connection);
        sender->initialize(name, descriptor);
        m_data->dialogs[idx] = sender;
#endif

        // Window title
        m_data->dialogs[idx]->setWindowTitle(
                QString("%1: %2")
                    .arg(m_data->connection->name())
                    .arg(name));

        QMainWindow * mainWindow = qobject_cast< QMainWindow * >(parent());
        QMdiArea * mdiArea = 
            qobject_cast< QMdiArea * >(mainWindow->centralWidget());

        QMdiSubWindow * subWindow = new QMdiSubWindow(mdiArea);
        subWindow->setWidget(m_data->dialogs[idx]);
        mdiArea->addSubWindow(subWindow);
        m_data->subwindows[idx] = subWindow;

    }
    m_data->dialogs[idx]->show();
    m_data->subwindows[idx]->show();
    m_data->subwindows[idx]->raise();
}

void ConnectionView::showConfig()
{
    // Por si está oculto
    m_data->connectionDockWidget->show();

    // Por si está debajo de otro
    m_data->connectionDockWidget->raise();
}

void ConnectionView::load(const QVariant& settings)
{
    const QVariantMap map = settings.toMap();
    const QVariantMap send = map["Send messages"].toMap();

    const ::gsim::core::descriptor_group_ptr out_ = 
        m_data->connection->descriptor()->out();

    for (unsigned int i = 0; i < out_->get_descriptor_count(); i++) 
    {
        const QString name = 
            getMessageName(out_->get_descriptor_by_index(i));

        const QVariantMap::const_iterator it = send.find(name);

        if (it != send.end())
        {
            // No debería mostrarse, solo cargar la configuración
            showSenderDialog((int) i);

            if (m_data->dialogs[i])
            {
                MessageSender * sender = 
                    static_cast< MessageSender * >(m_data->dialogs[i]);

                sender->load(it.value());
            }
        }
    }

    // recupera la configuración de la conexión
    m_data->connectionWidget->load(map["configuration"]);
}

void ConnectionView::save(QVariant& settings)
{
    QVariantMap map;
    QVariantMap send;

    for (unsigned int i = 0; i < m_data->dialogs.size(); i++) 
    {
        if (m_data->dialogs[i])
        {
            MessageSender * sender = 
                static_cast< MessageSender * >(m_data->dialogs[i]);

            const QString name = 
                getMessageName(sender->getReflective());

            sender->save(send[name]);
        }
    }

    map["Send messages"] = send;

    // salva la configuración de la conexión
    m_data->connectionWidget->save(map["configuration"]);

    settings = map;
}

void ConnectionView::loadStatus()
{
    const QString file = 
        QFileDialog::getOpenFileName(0,
                "Select a file", ".",
                tr("GSIM status file (*.status)"));

    if (!file.isEmpty())
    {
        // es thread-safe
        m_data->connection->statusManager()->restore(file);
    }
}

void ConnectionView::saveStatus()
{
    QString file = 
        QFileDialog::getSaveFileName(0, 
                "Select a file", ".",
                tr("GSIM status file (*.status)"));

    if (!file.isEmpty())
    {
        if(!file.endsWith(".status"))
            file.append(".status");

        // es thread-safe
        m_data->connection->statusManager()->dump(file);
    }
}

