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

