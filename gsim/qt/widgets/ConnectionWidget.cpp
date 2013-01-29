// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * ConnectionWidget.cpp
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

#include "ConnectionWidget.hpp"
#include <gsim/qt/widgets/FormWidget.hpp>
#include <gsim/qt/widgets/Status.hpp>
#include <gsim/qt/widgets/StartStopButton.hpp>
#include <limits>
#include <iostream>

using namespace gsim::qt;

ConnectionWidget::ConnectionWidget(QWidget * parent) :
    QWidget(parent)
{
}

ConnectionWidget::~ConnectionWidget()
{
}

void ConnectionWidget::save(QVariant& settings)
{
}

void ConnectionWidget::load(const QVariant& settings)
{
}

void ConnectionWidget::statusChanged(ConnectionStatus)
{
}

void ConnectionWidget::configChanged(ConnectionConfig_ptr)
{
}

//
// ConnectionWidgetFactory 
//

class ConnectionWidgetFactory::Data
{
public:

    typedef std::map< QString, factory_t > factories_t;

    factories_t factories;
};

ConnectionWidgetFactory::ConnectionWidgetFactory() :
    m_data(new Data)
{
    registerFactory("TCP", &TCPConnectionWidget::create);
    registerFactory("TCP/IP", &TCPConnectionWidget::create); // alias
    registerFactory("UDP", &UDPConnectionWidget::create);
    registerFactory("UDP/MCast", &UDPMulticastConnectionWidget::create);
    registerFactory("UDP/Multicast", &UDPMulticastConnectionWidget::create);
    registerFactory("MCast", &UDPMulticastConnectionWidget::create);
    registerFactory("Multicast", &UDPMulticastConnectionWidget::create);
    registerFactory("MultiCast", &UDPMulticastConnectionWidget::create);
    registerFactory("multicast", &UDPMulticastConnectionWidget::create);
}
    
ConnectionWidgetFactory::~ConnectionWidgetFactory()
{
    delete m_data;
}

ConnectionWidgetFactory * ConnectionWidgetFactory::instance()
{
    static std::auto_ptr< ConnectionWidgetFactory > _instance(
            new ConnectionWidgetFactory);
    return _instance.get();
}

void ConnectionWidgetFactory::registerFactory(const QString& type, 
        factory_t factory)
{
    m_data->factories[type] = factory;
}

void ConnectionWidgetFactory::registerAlias(const QString& type, 
        const QString& orig)
{
    Data::factories_t::const_iterator it = m_data->factories.find(orig);

    if (it != m_data->factories.end())
    {
        m_data->factories[type] = it->second;
    }
}

ConnectionWidget * ConnectionWidgetFactory::createWidget(
        const QString& type, QWidget * parent) const
{
    Data::factories_t::const_iterator it = m_data->factories.find(type);

    if (it != m_data->factories.end() && it->second)
    {
        return (it->second)(parent);
    }

    // TCP por defecto
    return TCPConnectionWidget::create(parent);
}

//
// TCP
//

class TCPConnectionWidget::Data
{
public:

    FormWidget * form;
    QLineEdit * host;
    QSpinBox * port;
    QComboBox * mode;
    QCheckBox * reconnect;

    QDialogButtonBox * buttonBox;
    StartStopButton * button;
};

ConnectionWidget * TCPConnectionWidget::create(QWidget * parent)
{
    return new TCPConnectionWidget(parent);
}

TCPConnectionWidget::TCPConnectionWidget(QWidget * parent) :
    ConnectionWidget(parent), m_data(new Data)
{
    QVBoxLayout * layout = new QVBoxLayout();
    FormWidget * form = new FormWidget();
    m_data->form = form;
    form->setMaximumWidth(270);

    m_data->host = new QLineEdit();
    m_data->port = new QSpinBox();
    m_data->mode = new QComboBox();
    m_data->reconnect = new QCheckBox();

    form->addField("Host", m_data->host);
    form->addField("Port", m_data->port);
    form->addField("Mode", m_data->mode);
    form->addField("Reconnect", m_data->reconnect);

    m_data->mode->addItem("Client");
    m_data->mode->addItem("Server");

    m_data->port->setRange(std::numeric_limits< unsigned short >::min(),
            std::numeric_limits< unsigned short >::max());

    layout->setSizeConstraint(QLayout::SetFixedSize);
    layout->addWidget(form);

    // Valores por defecto
    m_data->host->setText("127.0.1.1");
    m_data->port->setValue(5678);
    m_data->reconnect->setChecked(true);

    // Button
    m_data->buttonBox = new QDialogButtonBox(this);
    m_data->button = new StartStopButton();
    m_data->buttonBox->addButton(m_data->button, 
            QDialogButtonBox::AcceptRole);
    layout->addWidget(m_data->buttonBox);

    connect(m_data->button, SIGNAL(toggled(bool)), 
            this, SLOT(startStop(bool)));

    setLayout(layout);
}

TCPConnectionWidget::~TCPConnectionWidget()
{
    delete m_data;
}

ConnectionConfig_ptr TCPConnectionWidget::config() const
{
    TCPConnectionConfig * cfg = new TCPConnectionConfig;

    cfg->host = m_data->host->text().toStdString();
    cfg->port = (unsigned short) m_data->port->value();
    cfg->isServer = (m_data->mode->currentIndex() != 0);
    cfg->reconnect = m_data->reconnect->isChecked();

    return ConnectionConfig_ptr(cfg);
}

void TCPConnectionWidget::startStop(bool start)
{
    ConnectionConfig_ptr cfg;

    if (start)
    {
        cfg = config();
    }

    emit applyConfig(cfg);

    m_data->form->setEnabled(!start);
}

void TCPConnectionWidget::save(QVariant& settings)
{
    QVariantMap map;
    map["host"] = m_data->host->text();
    map["port"] = m_data->port->value();
    map["mode"] = m_data->mode->currentText();
    map["reconnect"] = m_data->reconnect->isChecked();
    settings = map;
}

void TCPConnectionWidget::load(const QVariant& settings)
{
    const QVariantMap map = settings.toMap();

    m_data->host->setText(map["host"].toString());
    m_data->port->setValue(map["port"].toInt());

    const QString mode (map["mode"].toString());
    int idx = m_data->mode->findText(mode);
    if (idx > -1)
        m_data->mode->setCurrentIndex(idx);

    m_data->reconnect->setChecked(map["reconnect"].toBool());
}

void TCPConnectionWidget::statusChanged(ConnectionStatus status)
{
    if (status == kStatusDisconnected)
    {
        m_data->form->setEnabled(true);
        m_data->button->setChecked(false);
    }
}

void TCPConnectionWidget::configChanged(ConnectionConfig_ptr cfg)
{
    TCPConnectionConfig * tcpCfg = 
        dynamic_cast< TCPConnectionConfig * >(cfg.get());

    if (tcpCfg)
    {
        m_data->host->setText(tcpCfg->host.c_str());
        m_data->port->setValue(tcpCfg->port);
        m_data->mode->setCurrentIndex(((tcpCfg->isServer)? 1: 0));
        m_data->reconnect->setChecked(tcpCfg->reconnect);
    }
}

//
// UDP
//

class UDPConnectionWidget::Data
{
public:
    FormWidget * form;
    QLineEdit * localHost;
    QSpinBox * localPort;
    QLineEdit * remoteHost;
    QSpinBox * remotePort;

    QDialogButtonBox * buttonBox;
    StartStopButton * button;
};

UDPConnectionWidget::UDPConnectionWidget(QWidget * parent) :
    ConnectionWidget(parent), m_data(new Data)
{
    QVBoxLayout * layout = new QVBoxLayout();
    FormWidget * form = new FormWidget();
    m_data->form = form;
    form->setMaximumWidth(270);

    m_data->localHost = new QLineEdit();
    m_data->localPort = new QSpinBox();
    m_data->remoteHost = new QLineEdit();
    m_data->remotePort = new QSpinBox();

    form->addField("Local host", m_data->localHost);
    form->addField("Local port", m_data->localPort);
    form->addField("Remote host", m_data->remoteHost);
    form->addField("Remote port", m_data->remotePort);

    m_data->localPort->setRange(
            std::numeric_limits< unsigned short >::min(),
            std::numeric_limits< unsigned short >::max());
    m_data->remotePort->setRange(
            std::numeric_limits< unsigned short >::min(),
            std::numeric_limits< unsigned short >::max());

    layout->setSizeConstraint(QLayout::SetFixedSize);
    layout->addWidget(form);

    // Valores por defecto
    m_data->localHost->setText("127.0.1.1");
    m_data->localPort->setValue(5679);
    m_data->remoteHost->setText("127.0.1.1");
    m_data->remotePort->setValue(5678);

    // Button
    m_data->buttonBox = new QDialogButtonBox(this);
    m_data->button = new StartStopButton();
    m_data->buttonBox->addButton(m_data->button, 
            QDialogButtonBox::AcceptRole);
    layout->addWidget(m_data->buttonBox);

    connect(m_data->button, SIGNAL(toggled(bool)), 
            this, SLOT(startStop(bool)));

    setLayout(layout);

}

UDPConnectionWidget::~UDPConnectionWidget()
{
    delete m_data;
}

ConnectionConfig_ptr UDPConnectionWidget::config() const
{
    UDPConnectionConfig * cfg = new UDPConnectionConfig;

    cfg->localHost = m_data->localHost->text().toStdString();
    cfg->localPort = (unsigned short) m_data->localPort->value();
    cfg->remoteHost = m_data->remoteHost->text().toStdString();
    cfg->remotePort = (unsigned short) m_data->remotePort->value();

    return ConnectionConfig_ptr(cfg);
}

ConnectionWidget * UDPConnectionWidget::create(QWidget * parent)
{
    return new UDPConnectionWidget(parent);
}

void UDPConnectionWidget::startStop(bool start)
{
    ConnectionConfig_ptr cfg;

    if (start)
    {
        cfg = config();
    }

    emit applyConfig(cfg);

    m_data->form->setEnabled(!start);
}

void UDPConnectionWidget::save(QVariant& settings)
{
    QVariantMap map;
    map["local host"] = m_data->localHost->text();
    map["local port"] = m_data->localPort->value();
    map["remote host"] = m_data->remoteHost->text();
    map["remote port"] = m_data->remotePort->value();
    settings = map;
}

void UDPConnectionWidget::load(const QVariant& settings)
{
    const QVariantMap map = settings.toMap();

    m_data->localHost->setText(map["local host"].toString());
    m_data->localPort->setValue(map["local port"].toInt());
    m_data->remoteHost->setText(map["remote host"].toString());
    m_data->remotePort->setValue(map["remote port"].toInt());
}

void UDPConnectionWidget::statusChanged(ConnectionStatus status)
{
    if (status == kStatusDisconnected)
    {
        m_data->form->setEnabled(true);
        m_data->button->setChecked(false);
    }
}

void UDPConnectionWidget::configChanged(ConnectionConfig_ptr cfg)
{
    UDPConnectionConfig * udpCfg = 
        dynamic_cast< UDPConnectionConfig * >(cfg.get());

    if (udpCfg)
    {
        m_data->localHost->setText(udpCfg->localHost.c_str());
        m_data->localPort->setValue(udpCfg->localPort);
        m_data->remoteHost->setText(udpCfg->remoteHost.c_str());
        m_data->remotePort->setValue(udpCfg->remotePort);
    }
}

//
// Multicast
//

class UDPMulticastConnectionWidget::Data
{
public:
    FormWidget * form;
    QLineEdit * localHost;
    QLineEdit * multicastAddress;
    QSpinBox * port;

    QDialogButtonBox * buttonBox;
    StartStopButton * button;
};

UDPMulticastConnectionWidget::UDPMulticastConnectionWidget(
        QWidget * parent) :
    ConnectionWidget(parent), m_data(new Data)
{
    QVBoxLayout * layout = new QVBoxLayout();
    FormWidget * form = new FormWidget();
    m_data->form = form;

    m_data->localHost = new QLineEdit();
    m_data->multicastAddress = new QLineEdit();
    m_data->port = new QSpinBox();

    form->addField("Local host", m_data->localHost);
    form->addField("Multicast address", m_data->multicastAddress);
    form->addField("Port", m_data->port);

    m_data->port->setRange(std::numeric_limits< unsigned short >::min(),
            std::numeric_limits< unsigned short >::max());

    layout->setSizeConstraint(QLayout::SetFixedSize);
    layout->addWidget(form);

    // Valores por defecto
    m_data->localHost->setText("0.0.0.0");
    m_data->multicastAddress->setText("225.0.0.1");
    m_data->port->setValue(5678);

    // Button
    m_data->buttonBox = new QDialogButtonBox(this);
    m_data->button = new StartStopButton();
    m_data->buttonBox->addButton(m_data->button, 
            QDialogButtonBox::AcceptRole);
    layout->addWidget(m_data->buttonBox);

    connect(m_data->button, SIGNAL(toggled(bool)), 
            this, SLOT(startStop(bool)));

    setLayout(layout);
}

UDPMulticastConnectionWidget::~UDPMulticastConnectionWidget()
{
    delete m_data;
}

ConnectionConfig_ptr UDPMulticastConnectionWidget::config() const
{
    UDPMulticastConnectionConfig * cfg = new UDPMulticastConnectionConfig;

    cfg->localHost = m_data->localHost->text().toStdString();
    cfg->multicastAddress = 
        m_data->multicastAddress->text().toStdString();
    cfg->port = (unsigned short) m_data->port->value();

    return ConnectionConfig_ptr(cfg);
}

ConnectionWidget * UDPMulticastConnectionWidget::create(QWidget * parent)
{
    return new UDPMulticastConnectionWidget(parent);
}

void UDPMulticastConnectionWidget::startStop(bool start)
{
    ConnectionConfig_ptr cfg;

    if (start)
    {
        cfg = config();
    }

    emit applyConfig(cfg);

    m_data->form->setEnabled(!start);
}

void UDPMulticastConnectionWidget::save(QVariant& settings)
{
    QVariantMap map;
    map["local host"] = m_data->localHost->text();
    map["port"] = m_data->port->value();
    map["multicast address"] = m_data->multicastAddress->text();
    settings = map;
}

void UDPMulticastConnectionWidget::load(const QVariant& settings)
{
    const QVariantMap map = settings.toMap();

    m_data->localHost->setText(map["local host"].toString());
    m_data->port->setValue(map["port"].toInt());
    m_data->multicastAddress->setText(
            map["multicast address"].toString());
}

void UDPMulticastConnectionWidget::statusChanged(ConnectionStatus status)
{
    if (status == kStatusDisconnected)
    {
        m_data->form->setEnabled(true);
        m_data->button->setChecked(false);
    }
}

void UDPMulticastConnectionWidget::configChanged(ConnectionConfig_ptr)
{
    // TODO
}

// 
// ConnectionStatus
//

class ConnectionStatusWidget::Data
{
public:
    Data(Connection_ptr con) :
        connection(con)
    {
    }

    Connection_ptr connection;
    Status * statusWidget;
};

ConnectionStatusWidget::ConnectionStatusWidget(Connection_ptr con, 
        QWidget * parent) :
    QWidget(parent), m_data(new Data(con))
{
    QHBoxLayout * layout = new QHBoxLayout();
    m_data->statusWidget = new Status(this);
    layout->addWidget(m_data->statusWidget);
    setLayout(layout);

    // Actualizaciones de estado
    connect(con.get(), SIGNAL(statusChanged(ConnectionStatus)),
            this, SLOT(update()));
    connect(con.get(), SIGNAL(nameChanged(const QString&)),
            this, SLOT(update()));

    // Inicializa el estado
    update();
}

ConnectionStatusWidget::~ConnectionStatusWidget()
{
    delete m_data;
}

void ConnectionStatusWidget::update()
{
    QString statusStr;

    switch(m_data->connection->status())
    {
        default:
        case kStatusDisconnected:
            statusStr = "Disconnected";
            m_data->statusWidget->setRedLight();
            break;

        case kStatusListening:
            statusStr = "Listening";
            m_data->statusWidget->setYellowLight();
            break;

        case kStatusConnected:
            statusStr = "Connected";
            m_data->statusWidget->setGreenLight();
            break;
    }

    // Actualiza el toolTip
    setToolTip(QString("Connection: %1, Status: %2")
            .arg(m_data->connection->name())
            .arg(statusStr));
}

