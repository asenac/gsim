// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * PlotTool.cpp
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

#include "PlotTool.hpp"
#include <gsim/qwt/SimplePlot.hpp>
#include <gsim/qwt/PlotModel.hpp>
#include <gsim/qt/proc/HolderEmitter.hpp>

#include <gsim/qt/widgets/SizeGrip.hpp>

#include <iostream>

using namespace gsim::qwt;

// Reflective plot

ReflectivePlot::ReflectivePlot(Connection_ptr connection_,
        TypeDescriptor_ptr message_,
        const QList< int >& path_, 
        QWidget * parent) :
    AbstractInputItem(connection_, message_, path_, parent)
{
    HolderEmitter * processor =
        new HolderEmitter(connection_, message_, path_);

    m_processor.reset(processor);

    // connect signal
    connect(processor,
            SIGNAL(append(Message_ptr,
                    Holder)),
            this,
            SLOT(appendValue(Message_ptr,
                    Holder)));

    // widget
    QVBoxLayout * layout = new QVBoxLayout();
    m_plot = new SimplePlot(this);
    layout->addWidget(m_plot);

    // Size grip
    qt::SizeGrip * sizeGrip = new qt::SizeGrip(this);
    sizeGrip->setAffectedWidget(m_plot);
    layout->addWidget(sizeGrip);

    layout->setMargin(0);

    // TODO start button

    setLayout(layout);
}

ReflectivePlot::~ReflectivePlot()
{
}

void ReflectivePlot::appendValue(Message_ptr req, 
        Holder hold)
{
    TypeDescriptor_ptr reflec = hold.get_type_descriptor();

    if (reflec->is_primitive())
    {
        m_plot->append(reflec->to_double(hold));
    }
    else if(reflec->is_repeated() && 
            reflec->get_slice()->is_primitive())
    {
        QVector< double > values;
        
        const unsigned int length = reflec->get_length(hold);
        TypeDescriptor_ptr slice = reflec->get_slice();

        for (unsigned int i = 0; i < length; i++) 
        {
            const core::holder h = reflec->get_child_value(hold, i);
            values.push_back(slice->to_double(h));
        }

        if (!values.isEmpty())
            m_plot->append(values);
    }
    // N-dimensional types
    else if(reflec->is_repeated() && 
            reflec->get_slice()->is_repeated())
    {
        unsigned int length = reflec->get_length(hold);

        for (unsigned int i = 0; i < length; i++) 
        {
            core::holder h = reflec->get_child_value(hold, i);

            // Recursive
            appendValue(req, h);
        }
    }
}

PlotTool::PlotTool(QWidget * parent) :
    AbstractInputTool(parent)
{
    setModel(createModel());
}

PlotTool::~PlotTool()
{
}

AbstractInputItem * PlotTool::createItem(
        Connection_ptr connection, 
        TypeDescriptor_ptr message,
        const QList< int >& path)
{
    return new ReflectivePlot(connection, message, path); 
}

MessageModel * PlotTool::createModel()
{
    return new PlotModel(this);
}

extern "C" 
{

    gsim::qt::AbstractInputTool * createPlotTool(
            QWidget * parent)
    {
        return new gsim::qwt::PlotTool(parent);
    }

} // extern C


