// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * ValueViewerTool.cpp
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

#include "ValueViewerTool.hpp"
#include <gsim/qt/proc/HolderEmitter.hpp>
#include <gsim/qt/reflective/path.hpp>
#include <gsim/qt/reflective/WidgetFactory.hpp>
#include <gsim/qt/reflective/DragAndDrop.hpp>
#include <gsim/qt/widgets/FormWidget.hpp>

using namespace gsim::qt;

// ValueViewer

ValueViewer::ValueViewer(Connection_ptr connection_,
        TypeDescriptor_ptr message_,
        const QList< int >& path_, 
        QWidget * parent) :
    AbstractInputItem(connection_, message_, path_, parent),
    m_widget(NULL),
    m_drag(NULL)
{
    TypeDescriptor_ptr descriptor = 
        followPath(message_, path_);

    QVBoxLayout * layout = new QVBoxLayout();
    FormWidget * form = new FormWidget(this);

    // Timestamp
    m_date = new QDateTimeEdit();
    m_date->setDisplayFormat("dd/MM/yyyy hh:mm:ss.zzz");
    m_date->setReadOnly(true);
    form->addField("Timestamp", m_date);

    // Value
    // Solo se crea si tiene sentido
    if (message_->get_children_count())
    {
        m_widget = WidgetFactoryRepository::create(descriptor, NULL);
        ReflectiveWidget * refWidget = 
            dynamic_cast< ReflectiveWidget * >(m_widget);

        bool isBig = false;

        if (refWidget)
        {
            isBig = refWidget->isBigWidget();

            refWidget->_setReadOnly(true);
        }

        if (isBig)
        {
            form->addBigField("Value", m_widget);
        }
        else
        {
            form->addField("Value", m_widget);
        }

        m_drag = new Drag(m_widget, this);
    }

    layout->addWidget(form);
    setLayout(layout);

    // Processor
    HolderEmitter * processor =
        new HolderEmitter(connection_, message_, path_);

    m_processor.reset(processor);

    // connect signal
    connect(processor,
            SIGNAL(append(Message_ptr, Holder)),
            this,
            SLOT(appendValue(Message_ptr, Holder)));
}

ValueViewer::~ValueViewer()
{
    delete m_drag;
}

void ValueViewer::start()
{
    emit addProcessor(m_processor);
}

void ValueViewer::appendValue(Message_ptr req, Holder hold)
{
    m_date->setDateTime(QDateTime::currentDateTime());

    if (m_widget)
    {
        ReflectiveWidget * w = dynamic_cast< ReflectiveWidget * >(m_widget);

        if (w)
            w->fromHolder(hold);
    }
}

//
//
// Tool
//
//

ValueViewerTool::ValueViewerTool(QWidget * parent) :
    AbstractInputTool(parent)
{
    // Todas las partes son chekeables
    m_model->setCheckable(true);
}

ValueViewerTool::~ValueViewerTool()
{
}

AbstractInputItem * ValueViewerTool::createItem(
        Connection_ptr connection, 
        TypeDescriptor_ptr message,
        const QList< int >& path)
{
    return new ValueViewer(connection, message, path);
}

// Save and load

void ValueViewer::save(QVariant& settings)
{
    QVariantMap map;

    settings = map;
}

void ValueViewer::load(const QVariant& settings)
{
    const QVariantMap map = settings.toMap();
}

