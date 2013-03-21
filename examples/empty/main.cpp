#include <gsim/qt/Application.hpp>

int main(int argc, char ** argv)
{
    // Inicialización
    gsim::qt::Application app(argc, argv);	

    // Ejecución
    return app.exec();
}

