// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * Connection.hpp
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

#ifndef GSIM_QT_CONNECTION_HPP
#define GSIM_QT_CONNECTION_HPP

#include <QtCore>
#include <gsim/qt/types.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <gsim/core/holder.hpp>
#include <gsim/core/event.hpp>

namespace gsim 
{
namespace qt 
{

/**
 * @brief Estados en los que se puede encontrar una conexión. 
 */
enum ConnectionStatus
{
    kStatusDisconnected,
    kStatusListening,
    kStatusConnected,
    kStatusMax
};

/**
 * @brief Abstracción para la configuración de una conexión.
 */
struct ConnectionConfig 
{
    /**
     * @brief Necesario para la vtable para hacer dynamic_cast.
     */
    virtual ~ConnectionConfig();
};

/**
 * @brief Puntero a una configuración de conexión que contendrán
 *        las señales y los slots.
 */
typedef boost::shared_ptr< ConnectionConfig > ConnectionConfig_ptr;


/**
 * @brief Configuración común de una conexión TCP/IP.
 */
struct TCPConnectionConfig : public ConnectionConfig
{
    std::string host;
    unsigned short port;
    bool isServer;
    bool reconnect;

    TCPConnectionConfig() 
    {}

    TCPConnectionConfig(const char * host_,
            unsigned short port_,
            bool isServer_,
            bool reconnect_) :
        host(host_), port(port_), 
        isServer(isServer_), reconnect(reconnect_)
    {}
};

/**
 * @brief Configuración común de una conexión UDP.
 */
struct UDPConnectionConfig : public ConnectionConfig
{
    std::string localHost;
    unsigned short localPort;
    std::string remoteHost;
    unsigned short remotePort;

    UDPConnectionConfig() 
    {}

    UDPConnectionConfig(const char * localHost_,
            unsigned short localPort_,
            const char * remoteHost_,
            unsigned short remotePort_) :
        localHost(localHost_), localPort(localPort_),
        remoteHost(remoteHost_), remotePort(remotePort_)
    {}
};

struct UDPMulticastConnectionConfig : public ConnectionConfig
{
    std::string localHost;
    std::string multicastAddress;
    unsigned short port;
};

class StatusManager;
class Controller;

/**
 * @brief Abstracción para la gestión de conexiones.
 *
 * Nota: evita emitir las señales de esta clase. Utiliza para tal fin
 * los métodos correspondientes que proporciona.
 */
class Connection : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString type READ type WRITE setType)
    Q_PROPERTY(ConnectionStatus status READ status WRITE setStatus)
public:

    Connection(ConnectionDescriptor_ptr descriptor = ConnectionDescriptor_ptr(),
            QObject * parent = 0);
    virtual ~Connection();

    QString name() const;
    QString type() const;
    ConnectionStatus status() const;

    /**
     * @brief Devuelve el controlador que está asociado a la conexión.
     *        Toda conexión ha de estado ligada a un controlador,
     *        el cual ha de ser su padre.
     *
     * @return El controlador asociado a la conexión.
     */
    Controller * controller() const;

    /**
     * @brief El descriptor de la conexión indica los mensajes
     *        que se envían y reciben por la misma. 
     *
     * @return El descriptor de la conexion. 
     */
    ConnectionDescriptor_ptr descriptor() const;

    ConnectionConfig_ptr config() const;

    void setStatusManager(StatusManager * manager);
    StatusManager * statusManager() const;

public slots:

    void setName(const QString&);
    void setType(const QString&);

    /**
     * @brief Envía el estado asociado a un mensaje utilizando 
     *        el método doSendMessage. Para poder enviar un estado
     *        la conexión tiene que tener un método registrado
     *        para el tipo del estado.
     *
     * @param message El mensaje a enviar.
     *
     * @return Devuelve si ha sido posible enviar el mensaje.
     */
    bool sendStatus(const QString& message);

    void sendMessage(Message_ptr);

    void setConfig(ConnectionConfig_ptr);

    /**
     * @brief Establece la configuración y asume la propiedad del 
     *        objeto configuración.
     *
     * @param cfg
     */
    void setConfig(ConnectionConfig * cfg);

    /**
     * @brief Intenta reenviar los mensajes recibidos en esta conexión
     *        por la conexión indicada.
     *
     * @param Connection_ptr con La conexión por donde se reenviarán.
     */
    void forwardTo(Connection_ptr con);

signals:

    void error(const QString&);
    void typeChanged(const QString&);
    void nameChanged(const QString&);
    void messageSent(Message_ptr);
    void messageReceived(Message_ptr);
    void statusChanged(ConnectionStatus);
    void configChanged(ConnectionConfig_ptr);

protected:

    /**
     * @brief Super-clase para enviar el contenido de un holder.
     */
    struct Functor 
    {
        
        /**
         * @brief Intenta realizar el envío del holder.
         *
         * @param h El holder a enviar.
         *
         * @return Devuelve si ha podido enviarse.
         */
        virtual bool operator()(core::holder h) = 0;
        virtual ~Functor();
    };

    /**
     * @brief Tipo para el intercambio de instancias de sub-clases
     *        de Functor.
     */
    typedef boost::shared_ptr< Functor > Functor_ptr;

    template< typename Value >
    struct FunctorImpl : public Functor
    {
        typedef boost::function< bool (const Value&) > method_t;
        method_t m_method;

        FunctorImpl(method_t m) :
            m_method(m)
        {
        }

        bool operator()(core::holder h)
        {
            const Value& v = h.to_value< Value >();
            return m_method(v);
        }
    };

    /**
     * @brief Registra un método para enviar los mensajes de tipo Value.
     *
     * Crea una instancia de FunctorImpl y la registra en una tabla interna
     * que asocia tipo con Functor. El método doSendMessage busca el Functor
     * en dicha tabla y realiza el envío.
     *
     * Method ha de ser un método de tipo:
     *
     * @code
     *
     * bool (This::*)(const Value&);
     *
     * @endcode
     *
     * O bien, por ser utilizado como tal.
     *
     * @tparam Value El tipo del mensaje.
     * @tparam This El tipo de la instancia de la clase que realiza el envío.
     *              Normalmente será una sub-clase de Connection.
     * @tparam Method El tipo del método. Ha de cumplir la restricción anterior.
     * @param m El puntero al método.
     * @param this_ La instancia con la que se invoca al método
     */
    template< typename Value, class This, typename Method>
    void registerMethod(Method m, This * this_)
    {
        Functor_ptr f(new FunctorImpl< Value >(
                    boost::bind(m, this_, _1)));

        doRegisterMethod(core::descriptor< Value >::instance(), f);
    }

    /**
     * @brief Registra un Functor para enviar los mensajes del tipo asociado
     * al descriptor.
     *
     * Puede ser un Functor propio o bien a través del método regsiterMethod.
     *
     * @param descriptor Ha de ser un descriptor de tipo. Nunca de miembro.
     * @param f Nunca ha de ser nulo.
     */
    void doRegisterMethod(core::descriptor_ptr descriptor, Functor_ptr f);


    /**
     * @brief Utiliza este método para cambiar el estado de la conexión.
     *
     * Emite la señal statusChanged si procede.
     *
     * @param ConnectionStatus status El nuevo estado.
     */
    void setStatus(ConnectionStatus status);


    /**
     * @brief Implementa este método para configurar la conexión.
     *
     * @param ConnectionConfig_ptr cfg Una instancia de un sub-tipo de 
     *        ConnectionConfig.
     *
     * @return Devuelve si la configuración ha podido ser aplicada.
     */
    virtual bool applyConfig(ConnectionConfig_ptr cfg) = 0;

    
    /**
     * @brief Método que será invocado desde el slot sendMessage.
     *
     * Reimplementa este método si quieres utilizar un mecanismo propio de
     * envío.
     *
     * Por defecto, busca en la tabla interna una instancia de Functor
     * asociada al tipo del holder del mensaje. Si la encuentra, devuelve
     * el valor devuelto por la llamada al Functor. En caso contrario
     * devuelve falso.
     *
     * @param Message_ptr msg
     *
     * @return Devuelve si el mensaje ha podido ser enviado.
     */
    virtual bool doSendMessage(Message_ptr msg);


    /**
     * @brief Utiliza este método para Notificar que un mensaje ha sido enviado.
     *
     * Nota: No emitas nunca la señal messageReceived. Hazlo siempre a
     * través de este método.
     *
     * @param Message_ptr msg El mensaje enviado. Nunca ha de ser nulo.
     */
    void notifyMessageReceived(Message_ptr msg);

    /**
     * @brief Notifica la recepción de un mensaje. Crea una instancia de Message_ptr y
     *        utiliza el método anterior.
     *
     * @tparam T El tipo de los datos recibidos.
     * @param messageName El nombre del mensaje.
     * @param data Los datos recibidos.
     */
    template< typename T >
    void notifyMessageReceived(const char * messageName, const T& data);

    class Data;
    Data * m_data;
};

template< typename T >
void Connection::notifyMessageReceived(const char * messageName, const T& data)
{
	using namespace gsim::core;

	holder srcHolder (create_holder< T >(const_cast< T& >(data)));
	holder dstHolder (srcHolder.clone());
	message_ptr msg(new message(messageName, dstHolder));
		
	notifyMessageReceived(msg);
}

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_CONNECTION_HPP */

