#include <gsim/qt/Application.hpp>
// Clase base para la conexión
#include <gsim/qt/model/GenericConnection.hpp>

int main(int argc, char ** argv)
{
    // Inicialización
    gsim::qt::Application app(argc, argv);	

    // Creamos la conexión
    gsim::qt::Connection_ptr connection(new gsim::qt::GenericConnection());

    // ... y la añadimos al controlador
    app.addConnection(connection);

    // Ejecución
    return app.exec();
}

