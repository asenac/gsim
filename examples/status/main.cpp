#include <gsim/qt/Application.hpp>
// Clase base para la conexión
#include <gsim/qt/model/GenericConnection.hpp>
#include <gsim/qt/model/StatusManager.hpp>

// Ficheros necesarios para generar los descriptores
// de los mensajes
#include <gsim/qt/model/Connection.ipp>

// Incluímos el fichero donde están definidos los mensajes
// y los tipos de datos utilizados en ellos.
#include "Types.hpp"
// ... y el fichero donde se 'adaptan' estos tipos.
#include "Types_adapted.hpp"

class StatusConnection : public gsim::qt::GenericConnection
{
public:

    StatusConnection() 
    {
        // Establece el tipo de la conexión
        setType("TCP");

        // Declaramos los mensajes que se recibirán por esta conexión
        descriptor()->in()->insert_descriptor< status::MyStatus >("STATUS_RESPONSE");
        descriptor()->in()->insert_descriptor< status::MyRequest >("STATUS_REQUEST");

        // Declaramos los mensajes que se enviarán por esta conexión
        descriptor()->out()->insert_descriptor< status::MyStatus >("STATUS_RESPONSE");
        descriptor()->out()->insert_descriptor< status::MyRequest >("STATUS_REQUEST");

        // Declaramos los métodos que se usarán para enviar los mensajes
        registerMethod< status::MyStatus >(&StatusConnection::sendMyStatus, this);
        registerMethod< status::MyRequest >(&StatusConnection::sendMyRequest, this);

        setStatusManager(new gsim::qt::StatusManager(this));

        statusManager()->defineStatus< status::MyStatus >("CURRENT STATUS");
    }

    // Método reimplementado para procesar los datos recibidos por la 
    // conexión
    std::size_t processData(const char * data, std::size_t size)
    {
        // Necesitamos más datos
        if (size < sizeof(status::Header))
            return 0;

        const status::Header * header = 
            reinterpret_cast< const status::Header * >(data);

        // Necesitamos más datos
        if (size < header->size)
            return 0;

        switch (header->id)
        {
            case status::ID_STATUS_REQUEST:
                {
                    const status::MyRequest msg = {};
                    notifyMessageReceived("STATUS_REQUEST", msg);

                    // Envía el estado actual
                    sendStatus("CURRENT STATUS");
                }
                break;

            case status::ID_STATUS_RESPONSE:
                if (header->size == (sizeof(status::Header) + sizeof(status::MyStatus)))
                {
                    const status::MyStatus * msg = 
                        reinterpret_cast< const status::MyStatus * >(
                                data + sizeof(status::Header));

                    notifyMessageReceived("STATUS_RESPONSE", *msg);
                }
                else
                {
                    emit error("Invalid response message.");
                }
                break;

            default:
                emit error("Unknown message.");
                break;
        }

        return size;
    }

    bool sendMyStatus(const status::MyStatus& msg)
    {
        const status::Header hdr = { 
            status::ID_STATUS_RESPONSE, 
            sizeof(status::Header) + sizeof(status::MyStatus)
        };

        send(reinterpret_cast< const char * >(&hdr), sizeof(status::Header));
        send(reinterpret_cast< const char * >(&msg), sizeof(status::MyStatus));

        return true;
    }

    bool sendMyRequest(const status::MyRequest& msg)
    {
        const status::Header hdr = { 
            status::ID_STATUS_REQUEST, 
            sizeof(status::Header)
        };

        send(reinterpret_cast< const char * >(&hdr), hdr.size);

        return true;
    }
};

typedef boost::shared_ptr< StatusConnection >
    StatusConnection_ptr;

int main(int argc, char ** argv)
{
    // Inicialización
    gsim::qt::Application app(argc, argv);	

    for (int i = 0; i < 2; i++) 
    {
        // Creamos la conexión
        StatusConnection_ptr connection(new StatusConnection());

        connection->setName(QString("Connection ") + QString::number(i));

        // ... y la añadimos al controlador
        app.addConnection(connection);
    }

    // Ejecución
    return app.exec();
}

