// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * PlotTool.hpp
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

#ifndef GSIM_QWT_REFLECTIVEPLOTTOOL_HPP
#define GSIM_QWT_REFLECTIVEPLOTTOOL_HPP

#include <gsim/qwt/export.hpp>
#include <gsim/qt/types.hpp>
#include <gsim/qt/tools/AbstractInputTool.hpp>

namespace gsim 
{
namespace qwt 
{

using namespace ::gsim::qt;

class SimplePlot;
class PlotModel;

class GSIM_QWT_DECLSPEC ReflectivePlot : 
    public AbstractInputItem
{
    Q_OBJECT
public:

    ReflectivePlot(Connection_ptr connection_,
            TypeDescriptor_ptr message_,
            const QList< int >& path_, 
            QWidget * parent = 0);
    virtual ~ReflectivePlot();

public slots:

    void appendValue(Message_ptr, Holder);

protected:

    SimplePlot * m_plot;
};

class GSIM_QWT_DECLSPEC PlotTool : public AbstractInputTool
{
    Q_OBJECT
public:

    PlotTool(QWidget * parent = 0);
    virtual ~PlotTool();

protected:

    AbstractInputItem * createItem(
            Connection_ptr connection, 
            TypeDescriptor_ptr message,
            const QList< int >& path);

    MessageModel * createModel();
};

} // namespace qwt
} // namespace gsim

#endif /* GSIM_QWT_REFLECTIVEPLOTTOOL_HPP */
