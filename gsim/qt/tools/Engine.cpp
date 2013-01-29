// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * Engine.cpp
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

#include "Engine.hpp"
#include <gsim/qt/proc/MessageProcessorManager.hpp>
#include <gsim/qt/tools/Sender.hpp>

#include <memory>

using namespace gsim::qt;

class Engine::Data
{
public:
    Data(Engine * this__) :
        this_(this__),
        inputMessageProcessorManager(new MessageProcessorManager),
        outputMessageProcessorManager(new MessageProcessorManager),
        senderController(new SenderController)
    {
        inputMessageProcessorManager->moveToThread(
                &inputMessageProcessorManagerThread);
        outputMessageProcessorManager->moveToThread(
                &outputMessageProcessorManagerThread);
        senderController->moveToThread(&senderControllerThread);

        inputMessageProcessorManagerThread.start();
        outputMessageProcessorManagerThread.start();
        senderControllerThread.start();
        senderController->start();
    }

    ~Data()
    {
        senderController->stop();
        inputMessageProcessorManagerThread.quit();
        outputMessageProcessorManagerThread.quit();
        senderControllerThread.quit();

        senderController->join();
        inputMessageProcessorManagerThread.wait();
        outputMessageProcessorManagerThread.wait();
        senderControllerThread.wait();

        delete inputMessageProcessorManager;
        delete outputMessageProcessorManager;
        delete senderController;
    }

    Engine * this_;

    QThread inputMessageProcessorManagerThread;
    MessageProcessorManager * inputMessageProcessorManager; 

    QThread outputMessageProcessorManagerThread;
    MessageProcessorManager * outputMessageProcessorManager; 

    QThread senderControllerThread;
    SenderController * senderController;
};

Engine::Engine() :
    m_data(new Data(this))
{
    connect(m_data->senderController,
            SIGNAL(error(const QString&)),
            this,
            SIGNAL(error(const QString&)));
}

Engine::~Engine()
{
    delete m_data;
}

Engine * Engine::instance()
{
    static std::auto_ptr< Engine > _instance(new Engine);
    return _instance.get();
}

QObject * Engine::inputMessageProcessorManager() const
{
    return m_data->inputMessageProcessorManager;
}

QObject * Engine::outputMessageProcessorManager() const
{
    return m_data->outputMessageProcessorManager;
}

QObject * Engine::senderController() const
{
    return m_data->senderController;
}

