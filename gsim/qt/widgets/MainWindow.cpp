// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * MainWindow.cpp
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

#include "MainWindow.hpp"
#include <gsim/qt/initialize.hpp>
#include <gsim/qt/widgets/ConnectionWidget.hpp>
#include <gsim/qt/widgets/ConnectionView.hpp>
#include <gsim/qt/widgets/TreeView.hpp>
#include <gsim/qt/item/LogModel.hpp>
#include <gsim/qt/item/ApplicationLogModel.hpp>
#include <gsim/qt/json.hpp>
#include <gsim/qt/item/MessageModel.hpp>
#include <gsim/qt/tools/AbstractTool.hpp>
#include <gsim/qt/tools/ValueViewerTool.hpp>
#include <gsim/qt/tools/DumpTool.hpp>
#include <gsim/qt/tools/SenderSequence.hpp>
#include <gsim/qt/tools/DetailedLog.hpp>
#include <gsim/qt/tools/Engine.hpp>
#include <gsim/version.hpp>

// TODO crear una clase LogView en condiciones
#include <gsim/qt/reflective/ReflectiveItemDelegate.hpp>

#include <map>
#include <fstream>

using namespace gsim::qt;

typedef std::map< Connection_ptr, ConnectionView_ptr > connections_t;

enum Tool
{
    kValueViewer,
    kPlotTool,
    kDumpTool,
    kSequenceTool,
    kToolMax
};

namespace  
{
    const char * toolTitles[] = {
        "Input value viewer",
        "Plot tool",
        "Dump tool",
        "Sequence tool"
    };

} // namespace 

class MainWindow::Data
{
public:
    MainWindow * this_;

    Data(MainWindow * this__) : 
        this_(this__),
        controller(NULL), 
        logModel(NULL),
        appLogModel(NULL),
        lastConnectionDock(NULL),
        aboutText("No description available.")
    {
        ::gsim::qt::initialize();

        tools.resize(kToolMax, NULL);
        toolWindows.resize(kToolMax, NULL);
    }

    ~Data()
    {
        delete detailedLog;
        delete logModel;
        delete appLogModel;

        for (unsigned int i = 0; i < tools.size(); i++) 
        {
            delete tools[i];
            delete toolWindows[i];
        }
    }

    Controller * controller;
    connections_t connections;

    // Widgets
    QMdiArea * mdiArea;

    // Menus
    QMenu * fileMenu;
    QMenu * connectionMenu;
    QMenu * toolsMenu;
    QMenu * windowMenu;
    QMenu * helpMenu;

    LogModel * logModel;
    DetailedLogController * detailedLog;

    ApplicationLogModel * appLogModel;

    MessageModel * inputMessageModel;
    MessageModel * outputMessageModel;

    QDockWidget * inDockWidget;
    QDockWidget * outDockWidget;

    QDockWidget * logDock;
    QDockWidget * appLogDock;
    QDockWidget * lastConnectionDock;

    QString aboutText;

    // Tools
    std::vector< AbstractTool * > tools;
    std::vector< QMdiSubWindow * > toolWindows;

    void createTool(Tool tool)
    {
        if (!tools[tool])
        {
            AbstractTool * toolInstance = NULL;
            switch(tool)
            {
                case kValueViewer:
                    toolInstance = new ValueViewerTool(this_); 
                    break;
                case kPlotTool:
                    createPlotTool();
                    toolInstance = tools[kPlotTool];
                    break;
                case kDumpTool:
                    toolInstance = new DumpTool(this_);
                    break;
                case kSequenceTool:
                    toolInstance = new SenderSequenceTool(this_);
                    break;
                default:
                    break;
            }

            if (toolInstance)
            {
                tools[tool] = toolInstance;
                toolWindows[tool] = new QMdiSubWindow(this_);
                toolWindows[tool]->setWidget(toolInstance);
                mdiArea->addSubWindow(toolWindows[tool]);
                toolWindows[tool]->setWindowTitle(toolTitles[tool]);

                connections_t::const_iterator it = connections.begin();

                for(; it != connections.end(); it++)
                {
                    toolInstance->registerInstance(it->first);
                }
            }
        }
    }

    void createPlotTool()
    {
        typedef AbstractInputTool* (*create_t)(QWidget*);

        if (!tools[kPlotTool])
        {
            // Loads the library
            QLibrary lib("gsim_qwt");
            lib.load();

            create_t create = (create_t) lib.resolve("createPlotTool");

            if (create)
            {
                // Creates the tool
                tools[kPlotTool] = create(this_);
            }
            else
            {
                QMessageBox::critical(this_, "Error initializing plot tool", 
                        "Unable to load gsim_qwt. "
                        "Ensure you have built gsim with qwt.");
            }
        }
    }

    void showTool(Tool tool)
    {
        createTool(tool);

        if (tools[tool])
        {
            tools[tool]->show();
            toolWindows[tool]->show();
            toolWindows[tool]->raise();
        }
    }
};

MainWindow::MainWindow(QWidget * parent, Qt::WindowFlags flags) :
    QMainWindow(parent, flags), m_data(new Data(this))
{
    {
        m_data->inputMessageModel = new MessageModel(MessageModel::kIn, this);
        m_data->inputMessageModel->setDisplayParameters(false);
        m_data->inDockWidget = new QDockWidget("Incoming Messages");
        QTreeView * view = new QTreeView();
        view->setExpandsOnDoubleClick(false);
        view->setModel(m_data->inputMessageModel);
        view->setHeaderHidden(true);
        m_data->inDockWidget->setWidget(view);
        addDockWidget(Qt::LeftDockWidgetArea, m_data->inDockWidget);

        QTimer::singleShot(100, view, SLOT(expandAll()));

        m_data->outputMessageModel = new MessageModel(MessageModel::kOut, this);
        m_data->outputMessageModel->setDisplayParameters(false);
        m_data->outDockWidget = new QDockWidget("Outgoing Messages");
        view = new QTreeView();
        view->setExpandsOnDoubleClick(false);
        view->setModel(m_data->outputMessageModel);
        view->setHeaderHidden(true);
        m_data->outDockWidget->setWidget(view);
        addDockWidget(Qt::LeftDockWidgetArea, m_data->outDockWidget);
        
        QTimer::singleShot(100, view, SLOT(expandAll()));

        connect(view, SIGNAL(doubleClicked(const QModelIndex&)),
                this, SLOT(outMessageClicked(const QModelIndex&)));

        tabifyDockWidget(m_data->outDockWidget, m_data->inDockWidget);

        // TODO se ocultan hasta que tengan utilidad
        m_data->inDockWidget->hide();
        // m_data->outDockWidget->hide();
    }

    // Central widget
    {
        m_data->mdiArea = new QMdiArea(this);
        setCentralWidget(m_data->mdiArea);
    }

    // Log
    {
        // Model
        m_data->logModel = new LogModel(this);

        m_data->detailedLog = new DetailedLogController(m_data->mdiArea);
        m_data->detailedLog->setLogModel(m_data->logModel);
        
        // View
        QTreeView * view = new TreeView();
        view->setModel(m_data->logModel);
        view->setDragDropMode(QAbstractItemView::DragOnly);
        view->setDragEnabled(true);

#if 1
        connect(view, 
                SIGNAL(doubleClicked(const QModelIndex&)),
                m_data->detailedLog, 
                SLOT(showDetails(const QModelIndex&)));
#else
        // TODO temporal, prueba de concepto
        m_data->logModel->setEditable(true);
#endif
        view->setItemDelegate(new ReflectiveItemDelegate(this));

        // Column Width
        view->setColumnWidth(0, 450);

        m_data->logDock = new QDockWidget("Log");
        m_data->logDock->setWidget(view);
        addDockWidget(Qt::BottomDockWidgetArea, m_data->logDock);
    }

    // Application log
    {
        m_data->appLogModel = new ApplicationLogModel(this);

        QTreeView * view = new TreeView();
        view->setModel(m_data->appLogModel);

        // Column Width
        view->setColumnWidth(0, 450);

        m_data->appLogDock = new QDockWidget("Application log");
        m_data->appLogDock->setWidget(view);
        addDockWidget(Qt::BottomDockWidgetArea, m_data->appLogDock);
        
        tabifyDockWidget(m_data->appLogDock, m_data->logDock);
    }

    setStatusBar(new QStatusBar());

    // Load configuration
    QAction * loadConfigurationAction = new QAction(
            style()->standardIcon(QStyle::SP_DialogOpenButton),
            "&Load configuration", this);
    connect(loadConfigurationAction, SIGNAL(triggered()), 
            this, SLOT(loadConfiguration()));

    // Save configuration
    QAction * saveConfigurationAction = new QAction(
            style()->standardIcon(QStyle::SP_DialogSaveButton),
            "&Save configuration", this);
    connect(saveConfigurationAction, SIGNAL(triggered()), 
            this, SLOT(saveConfiguration()));

    // Menus
    {
        m_data->fileMenu = menuBar()->addMenu("&File");
        m_data->fileMenu->addAction(loadConfigurationAction);
        m_data->fileMenu->addAction(saveConfigurationAction);
        m_data->fileMenu->addSeparator();
        QAction * closeAction = 
            m_data->fileMenu->addAction("&Exit", this, SLOT(close()));
        closeAction->setShortcut(QKeySequence::Close);

        m_data->connectionMenu = menuBar()->addMenu("&Connections");
        m_data->connectionMenu->addSeparator();

        // Tools
        m_data->toolsMenu = menuBar()->addMenu("&Tools");
        {
            m_data->toolsMenu->addAction("&Input value viewer", 
                    this, SLOT(showValueViewer()));
            m_data->toolsMenu->addAction("&Plot tool", 
                    this, SLOT(showPlotTool()));
            m_data->toolsMenu->addAction("&Dump tool", 
                    this, SLOT(showDumpTool()));

            m_data->toolsMenu->addSeparator();

            m_data->toolsMenu->addAction("&Sequence tool", 
                    this, SLOT(showSequenceTool()));
        }

        // Window
        m_data->windowMenu = menuBar()->addMenu("&Window");
        {
            QAction * showLogAction = 
                m_data->windowMenu->addAction("Show log", 
                        m_data->logDock, SLOT(show()));
            connect(showLogAction, SIGNAL(triggered()), 
                    m_data->logDock, SLOT(raise()));
            m_data->windowMenu->addAction("Clear log", 
                    m_data->logModel, SLOT(clearLog()));

            // Set max log size
            QAction * setMaxLogSizeAction = new QAction(
                    "Set log size", this);
            connect(setMaxLogSizeAction, SIGNAL(triggered()), 
                    this, SLOT(showSetMaxLogSize()));
            m_data->windowMenu->addAction(setMaxLogSizeAction);

            m_data->windowMenu->addSeparator();

            // Application log
            {
                QAction * showLogAction = 
                    m_data->windowMenu->addAction("Show application log", 
                            m_data->appLogDock, SLOT(show()));
                connect(showLogAction, SIGNAL(triggered()), 
                        m_data->appLogDock, SLOT(raise()));
                m_data->windowMenu->addAction("Clear application log", 
                        m_data->appLogModel, SLOT(clearLog()));
            }
            
            m_data->windowMenu->addSeparator();

            QAction * action = NULL;

            action = m_data->windowMenu->addAction("Inconming messages",
                    m_data->inDockWidget, SLOT(show()));
            connect(action, SIGNAL(triggered()), m_data->inDockWidget,
                    SLOT(raise()));

            action = m_data->windowMenu->addAction("Outgoing messages",
                    m_data->outDockWidget, SLOT(show()));
            connect(action, SIGNAL(triggered()), m_data->outDockWidget,
                    SLOT(raise()));
        }

        m_data->helpMenu = menuBar()->addMenu("&Help");
        m_data->helpMenu->addAction("&About GSIM", this, 
                SLOT(showAboutGSIM()));
        m_data->helpMenu->addAction("&About this simulator", this, 
                SLOT(showAbout()));

        QToolBar * toolBar = addToolBar("File");
        toolBar->addAction(loadConfigurationAction);
        toolBar->addAction(saveConfigurationAction);
    }

    setWindowTitle("Generic simulator");

    // Para mostrar los mensajes/errores del motor
    // de las herramientas
    connect(Engine::instance(), 
            SIGNAL(error(const QString&)),
            this,
            SLOT(displayError(const QString&)));
    connect(Engine::instance(), 
            SIGNAL(message(const QString&)),
            this,
            SLOT(displayMessage(const QString&)));
}

MainWindow::~MainWindow()
{
    delete m_data;
}

void MainWindow::setController(Controller * controller)
{
    if (m_data->controller)
    {
        // TODO disconnect signals
        disconnect(m_data->controller,
                SIGNAL(connectionCreated(Connection_ptr)),
                this,
                SLOT(connectionCreated(Connection_ptr)));

        disconnect(m_data->controller, 
                SIGNAL(messageSent(Connection_ptr, Message_ptr)),
                m_data->logModel, 
                SLOT(outputMessage(Connection_ptr, Message_ptr)));
        disconnect(m_data->controller, 
                SIGNAL(messageReceived(Connection_ptr, Message_ptr)),
                m_data->logModel, 
                SLOT(inputMessage(Connection_ptr, Message_ptr)));

        disconnect(m_data->controller, 
                SIGNAL(error(const QString&)),
                this,
                SLOT(displayError(const QString&)));
        disconnect(m_data->controller, 
                SIGNAL(message(const QString&)),
                this,
                SLOT(displayMessage(const QString&)));

        // TODO m_data->inputMessageModel->clear();
        // TODO m_data->outputMessageModel->clear();
    }
    
    m_data->controller = controller;

    if (m_data->controller)
    {
        // TODO connect signals
        connect(m_data->controller,
                SIGNAL(connectionCreated(Connection_ptr)),
                this,
                SLOT(connectionCreated(Connection_ptr)));

        connect(m_data->controller, 
                SIGNAL(messageSent(Connection_ptr, Message_ptr)),
                m_data->logModel, 
                SLOT(outputMessage(Connection_ptr, Message_ptr)));
        connect(m_data->controller, 
                SIGNAL(messageReceived(Connection_ptr, Message_ptr)),
                m_data->logModel, 
                SLOT(inputMessage(Connection_ptr, Message_ptr)));

        connect(m_data->controller, 
                SIGNAL(error(const QString&)),
                this,
                SLOT(displayError(const QString&)));
        connect(m_data->controller, 
                SIGNAL(message(const QString&)),
                this,
                SLOT(displayMessage(const QString&)));
    }
}

void MainWindow::connectionCreated(Connection_ptr con)
{
    connections_t::iterator it = m_data->connections.find(con);

    if (it == m_data->connections.end())
    {
        ConnectionView_ptr view(new ConnectionView(con, this));

        m_data->connections.insert(std::make_pair(con, view));

        // submenu del menu de conexiones
        m_data->connectionMenu->addMenu(view->menu());

        // Icono en la barra de estado
        statusBar()->addPermanentWidget(view->statusWidget());

        // Connection widget
        QDockWidget * connectionDock = view->connectionDockWidget();
        addDockWidget(Qt::LeftDockWidgetArea, connectionDock);

        if (m_data->lastConnectionDock)
            tabifyDockWidget(m_data->lastConnectionDock, connectionDock);

        m_data->lastConnectionDock = connectionDock;

        m_data->inputMessageModel->registerInstance(con);
        m_data->outputMessageModel->registerInstance(con);
    }
}

QMdiArea * MainWindow::mdiArea() const
{
    return m_data->mdiArea;
}

void MainWindow::showValueViewer()
{
    m_data->showTool(kValueViewer);
}

void MainWindow::showPlotTool()
{
    m_data->showTool(kPlotTool);
}

void MainWindow::showDumpTool()
{
    m_data->showTool(kDumpTool);
}

void MainWindow::showSequenceTool()
{
    m_data->showTool(kSequenceTool);
}

void MainWindow::setAboutText(const QString& text)
{
    m_data->aboutText = text;
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, "About this simulator", 
            m_data->aboutText);
}

void MainWindow::showAboutGSIM()
{
    static const char * aboutText = 
        "GSIM Version " GSIM_VERSION "\n"
        "Build " __DATE__ "\n"
        "Developed by: \n Andres Senac <a.senac@electronica-submarina.com>";

    QMessageBox::about(this, "About GSIM", 
            aboutText);
}

void MainWindow::showSetMaxLogSize()
{   
    bool ok = false;
    int res = 
        QInputDialog::getInt(this, 
                "Maximum log size",
                "Insert the log maximum size", 
                m_data->logModel->maxEntries(),
                1, 10000, 1, &ok);

    if (ok)
    {
        m_data->logModel->setMaxEntries(res);
    }
}

void MainWindow::outMessageClicked(const QModelIndex& index)
{
    if (m_data->outputMessageModel->isMessageNode(index))
    {
        Connection_ptr con(m_data->outputMessageModel->getInstance(index));

        connections_t::iterator it;

        if (con && ((it = m_data->connections.find(con)) 
                    != m_data->connections.end()))
        {
            it->second->showSenderDialog(index.row());
        }
    }
    else if (m_data->outputMessageModel->isGroupNode(index))
    {
        Connection_ptr con(m_data->outputMessageModel->getInstance(index));

        connections_t::iterator it;

        if (con && ((it = m_data->connections.find(con)) 
                    != m_data->connections.end()))
        {
            it->second->showConfig();
        }
    }
}

void MainWindow::loadConfiguration()
{
    const QString file = 
        QFileDialog::getOpenFileName(0,
                "Select a file", ".",
                tr("GSIM generic application configuration (*.cfg)"));

    if (!file.isEmpty())
    {
        QVariant var;

        // Try to Read a JSON file
        bool res = 
            fromJsonFile(file.toStdString().c_str(), var);

        if (res)
        {
            load(var);
        }
        else
        {
            QMessageBox::critical(this, 
                    "Error loading configuration", 
                    QString("Unable to load file ") +
                    file);
        }
    }
}

void MainWindow::saveConfiguration()
{
    QString file = 
        QFileDialog::getSaveFileName(0, 
                "Select a file", ".",
                tr("GSIM generic application configuration (*.cfg)"));

    if (!file.isEmpty())
    {
        if(!file.endsWith(".cfg"))
            file.append(".cfg");

        QVariant settings;
        save(settings);

        std::ofstream ofs(file.toStdString().c_str());
        json::ostream_writer_t ow(ofs, true);

        toJson(ow, settings);
    }
}

void MainWindow::load(const QVariant& settings)
{
    const QVariantMap map = settings.toMap();

    if (m_data->controller)
    {
        const QVariantMap connections = map["Connections"].toMap();

        for (QVariantMap::const_iterator it = connections.begin(); 
                it != connections.end(); ++it) 
        {
            // Obtiene la conexión a partir de su nombre
            const Connection_ptr con(m_data->controller->getConnection(it.key()));

            if (con)
            {
                // Busca la vista asocia a la conexión si la hubiera
                connections_t::iterator vi = m_data->connections.find(con);

                if (vi != m_data->connections.end())
                {
                    vi->second->load(it.value());
                }
            }
        }
    }

    for (int i = 0; i < kToolMax; i++) 
        if (map.contains(toolTitles[i]))
        {
            // Intenta crear la herramienta
            m_data->createTool(static_cast< Tool >(i));

            // Si la ha creado, carga su configuración.
            // El plot, por ejemplo, puede no cargarse
            if (m_data->tools[i])
                m_data->tools[i]->load(map[toolTitles[i]]);
        }
}

void MainWindow::save(QVariant& settings)
{
    QVariantMap map;
    QVariantMap connections;

    for (connections_t::const_iterator it = m_data->connections.begin(); 
            it != m_data->connections.end(); ++it) 
    {
        it->second->save(connections[it->first->name()]);
    }

    map["Connections"] = connections;

    // Tools
    for (int i = 0; i < kToolMax; i++) 
        if (m_data->tools[i]) 
            m_data->tools[i]->save(map[toolTitles[i]]);

    settings = map;
}

void MainWindow::displayMessage(const QString& msg)
{
    m_data->appLogModel->message(msg);
}

void MainWindow::displayError(const QString& err)
{
    m_data->appLogModel->error(err);

    // Muestra el log
    m_data->appLogDock->show();
    m_data->appLogDock->raise();
}

