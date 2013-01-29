// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * StatusManager.hpp
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

#ifndef GSIM_QT_STATUSMANAGER_HPP
#define GSIM_QT_STATUSMANAGER_HPP

#include <QtCore>
#include <boost/function.hpp>
#include <gsim/qt/types.hpp>
#include <gsim/core/holder.hpp>

namespace gsim 
{
namespace qt 
{

class StatusManager : public QObject
{
    Q_OBJECT
public:

    /**
     * @brief Prototipo para validar los estados, utilizados por el
     *        método applyStatus. 
     */
    typedef boost::function< bool(const QString&, Holder, Holder) >
        validator_t;

    StatusManager(QObject * parent = 0);
    virtual ~StatusManager();

    /**
     * @brief Devuelve una copia de un estado.
     *
     * @param name El nombre del estado.
     *
     * @return Puede ser un holder inválido si no existe tal estado.
     */
    Holder status(const QString& name);

    template< typename T >
    void setStatus(const QString& name, const T& t_);

    /**
     * @brief Devuelve la lista de estados de la conexión.
     *
     * @return La lista de estados.
     */
    QList< QString > statusList() const;

    /**
     * @brief Define un nuevo estado.
     *
     * @tparam T El tipo del estado
     * @param name El nombre del estado.
     */
    template< typename T >
    void defineStatus(const QString& name);

public slots:

    /**
     * @brief Establece el estado si puede aplicarse.
     *        Para determinar si puede aplicarse se invoca al 
     *        método applyStatus.
     *
     * @param name El nombre del estado.
     * @param holder Puede no ser del mismo tipo que el estado. Se utiliza
     *               la copia 'reflectiva'.
     */
    void setStatus(const QString& name, Holder holder);

    /**
     * @brief Vuelca el un fichero el estado actual de la conexión.
     *
     * @param filename
     */
    void dump(const QString& filename);

    /**
     * @brief Recupera desde fichero el estado de la conexión.
     *
     * @param filename
     */
    void restore(const QString& filename);

public:

    /**
     * @brief Devuelve una referencia al estado.
     *
     * @param name
     *
     * @return 
     */
    Holder statusRef(const QString& name);

    /**
     * @brief Devuelve la referencia al estado.
     *
     * @tparam T
     * @param name
     *
     * @return 
     */
    template < typename T >
    T& status(const QString& name);

signals:

    void statusChanged(const QString& name, Holder holder);
    void statusChanged(const QString& name, Holder older, Holder newer);
    void error(const QString& error);

protected:

    /**
     * @brief Comprueba si puede aplicarse el cambio de estado desde
     *        older a newer. 
     *
     * @param name El nombre del estado.
     * @param older El estado anterior.
     * @param newer El estado propuesto.
     *
     * @return Si es posible aplicar. Por defecto: true.
     */
    bool applyStatus(const QString& name, 
            Holder older, 
            Holder newer);

    /**
     * @brief No puede ser público ya que asume la propiedad del holder.
     *        En su defecto, hacer un defineStatus más un setStatus.
     *
     * @param name El nombre del estado.
     * @param value Un holder del tipo del estado.
     */
    void defineStatus(const QString& name, Holder value);

    class Data;
    Data * m_data;
};

template< typename T >
void StatusManager::setStatus(const QString& name, const T& t_)
{
    Holder holder( ::gsim::core::create_holder(const_cast< T& >(t_)));
    setStatus(name, holder);
}

template < typename T >
T& StatusManager::status(const QString& name)
{
    Holder holder( statusRef(name));
    return holder.to_value< T >();
}

template< typename T >
void StatusManager::defineStatus(const QString& name)
{
    Holder value( ::gsim::core::create_holder< T >());

    defineStatus(name, value);
}

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_STATUSMANAGER_HPP */

