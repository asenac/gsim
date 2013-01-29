// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * WidgetFactory.hpp
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

#ifndef GSIM_QT_WIDGETFACTORY_HPP
#define GSIM_QT_WIDGETFACTORY_HPP

#include <boost/function.hpp>

#include <gsim/qt/reflective/ReflectiveWidget.hpp>

class QWidget;

namespace gsim 
{
namespace qt 
{

/**
 * @brief Función para registar factorías de widgets en
 *        el repositorio de factorías.
 *
 * Una factoría nunca ha de devolver NULL. Ver método
 * createWidget de WidgetFactoryRepository.
 */
typedef 
    boost::function< QWidget * (
            TypeDescriptor_ptr, QWidget *) > 
    WidgetFactory_t;

/**
 * @brief Repositorio de factorías de widgets.
 */
class WidgetFactoryRepository
{
public:

    WidgetFactoryRepository();
    ~WidgetFactoryRepository();

    /**
     * @brief Establece la factoría de widgets por defecto.
     *
     * @param factory Ha de ser una factoría válida.
     */
    void setDefaultFactory(WidgetFactory_t factory);

    /**
     * @brief Registra una factoría con nombre.
     *
     * @param factoryName El nombre de la factoría.
     * @param factory Ha de ser una factoría válida.
     */
    void setFactory(const std::string& factoryName, 
            WidgetFactory_t factory);

    /**
     * @brief Crea un widget para un descriptor dado.
     *
     * @param descriptor Descriptor de un tipo o miembro.
     * @param parent Widget padre. Puede ser NULL.
     *
     * @return Nunca debe devolver NULL. Si es un descriptor
     *         de un tipo soportadao, el widget devuelto
     *         herederá de ReflectiveWidget. En caso contrario
     *         ha de devolver un QWidget que indique el error.
     */
    QWidget * createWidget(
            TypeDescriptor_ptr descriptor, 
            QWidget * parent) const;
    
    /**
     * @brief Instancia por defecto. Puede haber otras 
     *        instancias para propósitos particulares.
     *
     * @return La instancia por defecto.
     */
    static WidgetFactoryRepository * defaultInstance();

    static QWidget * create(
            TypeDescriptor_ptr descriptor, 
            QWidget * parent);

protected:
    class Data;
    Data * m_data;
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_WIDGETFACTORY_HPP */

