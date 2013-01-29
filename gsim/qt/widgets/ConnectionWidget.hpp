// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * ConnectionWidget.hpp
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

#ifndef GSIM_QT_CONNECTIONWIDGET_HPP
#define GSIM_QT_CONNECTIONWIDGET_HPP

#include <QWidget>
#include <boost/function.hpp>
#include <gsim/qt/model/Connection.hpp>

namespace gsim 
{
namespace qt 
{

class ConnectionWidget : public QWidget
{
    Q_OBJECT
public:

    ConnectionWidget(QWidget * parent = 0);
    virtual ~ConnectionWidget();

    virtual ConnectionConfig_ptr config() const = 0;

    virtual void save(QVariant& settings);
    virtual void load(const QVariant& settings);

public slots:

    virtual void statusChanged(ConnectionStatus);
    virtual void configChanged(ConnectionConfig_ptr);

signals:

    void applyConfig(ConnectionConfig_ptr);
};


/**
 * @brief Factoria para crear widgets para configurar las conexiones.
 *
 * Permite registrar nuevos tipos de conexiones.
 */
class ConnectionWidgetFactory
{
protected:

    ConnectionWidgetFactory();

public:

    typedef 
        boost::function< ConnectionWidget *(QWidget * parent) > 
        factory_t;
    
    ~ConnectionWidgetFactory();

    static ConnectionWidgetFactory * instance();

    void registerFactory(const QString& type, factory_t);

    void registerAlias(const QString& type, const QString& orig);

    ConnectionWidget * createWidget(const QString& type, 
            QWidget * parent) const;

protected:
    class Data;
    Data * m_data;
};

class TCPConnectionWidget : public ConnectionWidget
{
    Q_OBJECT
public:

    TCPConnectionWidget(QWidget * parent = 0);
    virtual ~TCPConnectionWidget();

    virtual ConnectionConfig_ptr config() const;

    virtual void save(QVariant& settings);
    virtual void load(const QVariant& settings);

    static ConnectionWidget * create(QWidget * parent);

public slots:

    void statusChanged(ConnectionStatus);
    void configChanged(ConnectionConfig_ptr);

protected slots:

    void startStop(bool start);

protected:

    class Data;
    Data * m_data;
};

class UDPConnectionWidget : public ConnectionWidget
{
    Q_OBJECT
public:
    UDPConnectionWidget(QWidget * parent = 0);
    virtual ~UDPConnectionWidget();

    virtual ConnectionConfig_ptr config() const;

    virtual void save(QVariant& settings);
    virtual void load(const QVariant& settings);
    
    static ConnectionWidget * create(QWidget * parent);

public slots:

    void statusChanged(ConnectionStatus);
    void configChanged(ConnectionConfig_ptr);

protected slots:

    void startStop(bool start);

protected:

    class Data;
    Data * m_data;
};

class UDPMulticastConnectionWidget : public ConnectionWidget
{
    Q_OBJECT
public:
    UDPMulticastConnectionWidget(QWidget * parent = 0);
    virtual ~UDPMulticastConnectionWidget();

    virtual ConnectionConfig_ptr config() const;

    virtual void save(QVariant& settings);
    virtual void load(const QVariant& settings);
    
    static ConnectionWidget * create(QWidget * parent);

public slots:

    void statusChanged(ConnectionStatus);
    void configChanged(ConnectionConfig_ptr);

protected slots:

    void startStop(bool start);

protected:

    class Data;
    Data * m_data;
};

class ConnectionStatusWidget : public QWidget
{
    Q_OBJECT
public:

    ConnectionStatusWidget(Connection_ptr con, QWidget * parent = 0);
    virtual ~ConnectionStatusWidget();

protected slots:

    void update();

protected:
    class Data;
    Data * m_data;
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_CONNECTIONWIDGET_HPP */

