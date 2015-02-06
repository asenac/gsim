// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * Application.cpp
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

#include "Application.hpp"
#include <gsim/qt/initialize.hpp>
#include <gsim/qt/model/Controller.hpp>
#include <gsim/qt/widgets/MainWindow.hpp>

#include <QApplication>

using namespace gsim::qt;

class Application::Data
{
public:
    Data(int& argc, char** argv) : app(argc, argv)
    {
        ::gsim::qt::initialize();

        mainWindow.setController(&controller);
    }

    QApplication app;
    Controller controller;
    MainWindow mainWindow;
};

Application::Application(int& argc, char** argv) : m_data(new Data(argc, argv))
{
}

Application::~Application()
{
    delete m_data;
}

void Application::addConnection(Connection_ptr connection)
{
    m_data->controller.addConnection(connection);
}

int Application::exec()
{
    m_data->controller.start();
    m_data->mainWindow.show();
    return m_data->app.exec();
}

MainWindow * Application::window() const
{
    return &m_data->mainWindow;
}

void Application::setTitle(const char * title)
{
   m_data->mainWindow.setWindowTitle(title);
}

void Application::setDescription(const char * description)
{
   m_data->mainWindow.setAboutText(description);
}
