// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * MainWindow.hpp
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

#ifndef GSIM_QT_WIDGETS_MAINWINDOW_HPP
#define GSIM_QT_WIDGETS_MAINWINDOW_HPP

#include <QtGui>
#include <gsim/qt/model/Controller.hpp>

namespace gsim 
{
namespace qt 
{

class Controller;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget * parent = 0, Qt::WindowFlags flags = 0);
    virtual ~MainWindow();


    /**
     * @brief Establece el controlador al que se conecta la 
     *        interfaz gráfica.
     *
     * @param controller El controlador.
     */
    void setController(Controller * controller);

    /**
     * @brief Devuelve el widget central de la ventana.
     *
     * @return Un widget de tipo QMdiArea. 
     */
    QMdiArea * mdiArea() const;

    void load(const QVariant& settings);

    void save(QVariant& settings);

protected slots:

    /**
     * @brief Slot conectado al controlador para notificar la creación
     *        de una conexión.
     *
     * @param Connection_ptr con La conexión creada.
     */
    void connectionCreated(Connection_ptr con);
    // void connectionDeleted(Connection_ptr);

    void outMessageClicked(const QModelIndex & index);

public slots:

    void setAboutText(const QString&);

    void showValueViewer();
    void showPlotTool();
    void showDumpTool();
    void showSequenceTool();
    void showAbout();
    void showAboutGSIM();

    void showSetMaxLogSize();

    void loadConfiguration();
    void saveConfiguration();

    void displayMessage(const QString& msg);
    void displayError(const QString& err);

protected:

    class Data;

    Data * m_data;
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_WIDGETS_MAINWINDOW_HPP */

