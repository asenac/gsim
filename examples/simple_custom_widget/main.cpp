#include <gsim/qt/Application.hpp>
// Clase base para la conexión
#include <gsim/qt/model/GenericConnection.hpp>

// Ficheros necesarios para generar los descriptores
// de los mensajes
#include <gsim/qt/model/Connection.ipp>

// Incluímos el fichero donde están definidos los mensajes
// y los tipos de datos utilizados en ellos.
#include "Types.hpp"
// ... y el fichero donde se 'adaptan' estos tipos.
#include "Types_adapted.hpp"

#include <gsim/qt/reflective/WidgetFactory.hpp>
#include <gsim/qt/reflective/CustomWidgets.hpp>

#include "ForbiddenStringWidget.hpp"

namespace
{
    GSIM_INITIALIZER()
    {
        // Registra una nueva factoría
        gsim::qt::WidgetFactoryRepository::defaultInstance()->setFactory("forbiddenwords",
                simple::ForbiddenStringWidget::create);

        // Asigna al campo name del mensaje HelloMsg las propiedades
        // necesarias para usar la nueva factoria
        GSIM_PROPERTY(simple::HelloMsg::name, "widget", std::string("forbiddenwords"));
        GSIM_PROPERTY(simple::HelloMsg::name, "value.forbidden", std::string("hola,adios"));
    }
}

class SimpleConnection : public gsim::qt::GenericConnection
{
public:

    SimpleConnection() 
    {
        // Establece el nombre de la conexión
        setName("Simple");

        // Establece el tipo de la conexión
        setType("UDP");

        // Declaramos los mensajes que se recibirán por esta conexión
        descriptor()->in()->insert_descriptor< simple::HelloMsg >("HELLO_MSG");

        // Declaramos los mensajes que se enviarán por esta conexión
        descriptor()->out()->insert_descriptor< simple::HelloMsg >("HELLO_MSG");

        // Declaramos los métodos que se usarán para enviar los mensajes
        registerMethod< simple::HelloMsg >(&SimpleConnection::sendHelloMsg, this);
    }

    // Método reimplementado para procesar los datos recibidos por la 
    // conexión
    std::size_t processData(const char * data, std::size_t size)
    {
        // Reconstriumos el mensaje
        simple::HelloMsg msg;
        msg.name = std::string(data, size);

        // Notificamos la recepción del mensaje
        notifyMessageReceived("HELLO_MSG", msg);
        
        // El mensaje entero se puede consumir
        return size;
    }

    bool sendHelloMsg(const simple::HelloMsg& msg)
    {
        // Método proporcionado para enviar datos crudos por el
        // socket de la conexión
        send(msg.name.c_str(), msg.name.size());

        return true;
    }
};

typedef boost::shared_ptr< SimpleConnection >
    SimpleConnection_ptr;

int main(int argc, char ** argv)
{
    // Inicialización
    gsim::qt::Application app(argc, argv);	

    // Creamos la conexión
    SimpleConnection_ptr connection(new SimpleConnection());

    // ... y la añadimos al controlador
    app.addConnection(connection);

    // Ejecución
    return app.exec();
}

