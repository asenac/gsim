// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * AbstractTool.hpp
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

#ifndef GSIM_QT_ABSTRACTTOOL_HPP
#define GSIM_QT_ABSTRACTTOOL_HPP

#include <QtGui>
#include <gsim/qt/export.hpp>
#include <gsim/qt/types.hpp>

namespace gsim 
{
namespace qt 
{

/**
 * @brief Abstraction for collections.
 */
class GSIM_QT_DECLSPEC AbstractTool : public QWidget
{
    Q_OBJECT
public:

    /**
     * @brief Functor for interation.
     *
     * Register instances in the tool.
     */
    struct Register
    {
        AbstractTool * _this;
        Register(AbstractTool * __this) : _this(__this) {}

        void operator()(Connection_ptr o)
        {
            _this->registerInstance(o);
        }
    };

    AbstractTool(QWidget * parent = 0);
    virtual ~AbstractTool();

    /**
     * @brief Use this method to save the tool's configuration.
     *
     * @param settings Recipient for the configuration.
     */
    virtual void save(QVariant& settings) = 0;

    /**
     * @brief Use this method to load the tool's configuration.
     *
     * @param settings Recipient for the configuration.
     */
    virtual void load(const QVariant& settings) = 0;


public slots:

    /**
     * @brief Register an instance in this tool.
     *
     * @param connection The instance.
     */
    virtual void registerInstance(Connection_ptr connection) = 0;

    /**
     * @brief Unregister an instance from this tool. 
     *
     * @param connection The instance.
     */
    virtual void unregisterInstance(Connection_ptr connection) = 0;

    /**
     * @brief Stop the tool. Reimplement if required. 
     */
    virtual void stop();
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_ABSTRACTTOOL_HPP */

