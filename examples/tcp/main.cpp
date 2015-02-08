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

#include <sstream>

class SimpleConnection : public gsim::qt::GenericConnection
{
public:

    SimpleConnection()
    {
        // Establece el nombre de la conexión
        setName("Simple");

        // Establece el tipo de la conexión
        setType("TCP");

        // Declaramos los mensajes que se recibirán por esta conexión
        descriptor()->in()->insert_descriptor< simple::HelloMsg >("HELLO_MSG");
        descriptor()->in()->insert_descriptor< simple::TimeMsg >("TIME_MSG");

        // Declaramos los mensajes que se enviarán por esta conexión
        descriptor()->out()->insert_descriptor< simple::HelloMsg >("HELLO_MSG");
        descriptor()->out()->insert_descriptor< simple::TimeMsg >("TIME_MSG");

        // Declaramos los métodos que se usarán para enviar los mensajes
        registerMethod< simple::HelloMsg >(&SimpleConnection::sendHelloMsg, this);
        registerMethod< simple::TimeMsg >(&SimpleConnection::sendTimeMsg, this);
    }

    // Método reimplementado para procesar los datos recibidos por la
    // conexión
    std::size_t processData(const char * data, std::size_t size)
    {
        std::size_t available = size;
        std::istringstream ss(std::string(data, size));
        if (available)
        {
            uint8_t type = 0;
            ss >> type;

            if (type == simple::TIME_MSG)
            {
                if (available >= 3)
                {
                    simple::TimeMsg msg;
                    ss >> msg.hours;
                    ss >> msg.minutes;
                    ss >> msg.seconds;

                    notifyMessageReceived("TIME_MSG", msg);
                }
            }
            else
            {
                uint32_t size_;

                if ((ss >> size_).good() && available > size_)
                {
                    simple::HelloMsg msg;
                    available -= 1 - 4 - size_;

                    msg.name.reserve(size_);
                    char c;

                    for (size_t i = 0; i < size_; i++)
                    {
                        ss >> c;
                        msg.name += c;
                    }

                    notifyMessageReceived("HELLO_MSG", msg);
                }
            }
        }

        return available;
    }

    bool sendHelloMsg(const simple::HelloMsg& msg)
    {
        std::ostringstream ss;
        ss << (uint8_t) simple::HELLO_MSG;
        ss << (uint32_t) msg.name.size();
        ss.write(msg.name.c_str(), msg.name.size());

        const std::string str = ss.str();
        send(str.c_str(), str.size());

        return true;
    }

    bool sendTimeMsg(const simple::TimeMsg& msg)
    {
        std::ostringstream ss;
        ss << (uint8_t) simple::TIME_MSG;
        ss << msg.hours;
        ss << msg.minutes;
        ss << msg.seconds;

        const std::string str = ss.str();
        send(str.c_str(), str.size());

        return true;
    }
};

typedef boost::shared_ptr<SimpleConnection> SimpleConnection_ptr;

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
