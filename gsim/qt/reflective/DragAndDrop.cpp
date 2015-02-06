// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * DragAndDrop.cpp
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

#include "DragAndDrop.hpp"
#include <gsim/qt/reflective/ReflectiveWidget.hpp>

#include <gsim/qt/constants.hpp>
#include <gsim/core/descriptor_utils.hpp>

#include <sstream>
#include <gsim/json/reflective.hpp>

using namespace gsim::qt;

namespace  
{
    bool supportDrag(QObject * w)
    {
        ReflectiveWidget * r = 
            dynamic_cast< ReflectiveWidget * >(w);

        if (r)
            return r->supportDrag();

        return w->isWidgetType()  
            && !dynamic_cast< QScrollBar * >(w) 
            && !dynamic_cast< QLineEdit * >(w);
    }
} // namespace 

class Drag::Data
{
public:

    Data(ReflectiveWidget * w, QWidget * qw) :
        widget(w), qwidget(qw), source(NULL),
        frame(new QWidget(qw)),
        currentReflective(NULL)
    {
        frame->setAttribute(Qt::WA_TransparentForMouseEvents);
        frame->setFocusPolicy(Qt::NoFocus);
        frame->resize(0,0);

        frame->setStyleSheet("border:5px solid rgb(0, 255, 0); margin: 0px;");
    }

    ReflectiveWidget * widget;
    QWidget * qwidget;

    QPoint dragStartPosition;
    QWidget * source;

    // Para dibujar el borde
    QWidget * frame;
    QWidget * currentReflective;
};

Drag::Drag(QWidget * refWidget, QObject * parent) :
    QObject(parent), 
    m_data(new Data(dynamic_cast< ReflectiveWidget * >(refWidget),
            refWidget))
{
    install(refWidget);
}

Drag::~Drag()
{
    delete m_data;
}

void Drag::install(QObject * w)
{
    if (supportDrag(w) && w != m_data->frame)
    {
        w->installEventFilter(this);

        typedef QList< QWidget * > list_t;
        list_t ch = w->findChildren< QWidget * >();

        for (list_t::iterator it = ch.begin(); it != ch.end(); ++it) 
        {
            if (supportDrag(*it) && *it != m_data->frame)
                (*it)->installEventFilter(this);
        }
    }
}

void Drag::dragLeaveEvent(QDragLeaveEvent *event)
{
}

bool Drag::eventFilter(QObject * obj, QEvent * event)
{
    if (event->type() == QEvent::ChildAdded)
    {
        QChildEvent *cEvent = static_cast< QChildEvent * >(event);
        
        childEvent(cEvent);
    }
    else if (event->type() == QEvent::DragLeave)
    {
        QDragLeaveEvent *dragEvent = static_cast< QDragLeaveEvent * >(event);

        dragLeaveEvent(dragEvent);

        return true;
    }
    else if (event->type() == QEvent::MouseMove)
    {
        QMouseEvent *cEvent = static_cast< QMouseEvent * >(event);

        if (m_data->source == m_data->qwidget->window()->childAt(cEvent->globalPos()))
            mouseMoveEvent(cEvent);

        // if (obj == m_data->qwidget)
        {
#if 0
            QWidget * w = qobject_cast< QWidget * >(obj);
            // QWidget * w = m_data->qwidget->childAt(cEvent->pos());

            // Es reflective
            while(w && w != m_data->qwidget && !dynamic_cast< ReflectiveWidget * >(w))
                w = w->parentWidget();

            if (w)
            {
                if (dynamic_cast< ReflectiveWidget * >(w))
                {
                    const QPoint pos = m_data->qwidget->mapFromGlobal(w->parentWidget()->mapToGlobal(w->pos()));

                    m_data->frame->setGeometry(w->rect());
                    m_data->frame->move(pos);
                }
            }
#endif
        }
    }
    else if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *cEvent = static_cast< QMouseEvent * >(event);

        mousePressEvent(cEvent);
    }

    return false;
}

void Drag::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_data->dragStartPosition = event->globalPos();
        m_data->source = m_data->qwidget->window()->childAt(event->globalPos());;
    }
}

void Drag::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_data->widget)
        return;

    if (!(event->buttons() & Qt::LeftButton))
        return;
    
    if ((event->globalPos() - m_data->dragStartPosition).manhattanLength()
            < QApplication::startDragDistance())
        return;

    TypeDescriptor_ptr descriptor = 
        m_data->widget->getReflective();
    
    Holder holder (descriptor->create_holder());
    m_data->widget->toHolder(holder);

    if (core::utils::calculate_size(holder) < GSIM_DRAG_MAX_SIZE)
    {
        QDrag * drag = new QDrag(m_data->qwidget);
        QMimeData * mimeData = new QMimeData;

        std::ostringstream oss;

        json::write(oss, descriptor, holder);

        mimeData->setText(oss.str().c_str());
        drag->setMimeData(mimeData);

        /* Qt::DropAction dropAction = */ drag->exec(Qt::CopyAction);
    }
    else
    {
        QMessageBox::information(m_data->qwidget, "Information", 
                "Drag events have been disabled for performance reasons.");
    }
}

void Drag::childEvent(QChildEvent * event)
{
    if (event->added())
    {
        install(event->child());
    }
}

//
// Drop
//

class Drop::Data
{
public:

    Data(ReflectiveWidget * w, QWidget * qw) :
        widget(w), qwidget(qw)
    {
    }

    ReflectiveWidget * widget;
    QWidget * qwidget;
};

//
// Drop
//

Drop::Drop(QWidget * refWidget, QObject * parent) :
    QObject(parent), 
    m_data(new Data(dynamic_cast< ReflectiveWidget * >(refWidget), 
            refWidget))
{
    install(refWidget);
}

Drop::~Drop()
{
    delete m_data;
}

bool Drop::eventFilter(QObject * obj, QEvent * event)
{
    if (event->type() == QEvent::ChildAdded)
    {
        QChildEvent *cEvent = static_cast< QChildEvent * >(event);
        
        childEvent(cEvent);
    }
    else if (event->type() == QEvent::DragEnter)
    {
        QDragEnterEvent *dragEvent = static_cast< QDragEnterEvent * >(event);

        dragEnterEvent(dragEvent);

        // return true;
    }
    else if (event->type() == QEvent::Drop)
    {
        QDropEvent *cEvent = static_cast< QDropEvent * >(event);

        dropEvent(cEvent);

        return true;
    }

    return false;
}

void Drop::dropEvent(QDropEvent *event)
{
    if (!m_data->widget)
        return;

    const std::string str = event->mimeData()->text().toStdString();

    try 
    {
        TypeDescriptor_ptr descriptor = 
            m_data->widget->getReflective();

        Holder holder = descriptor->create_holder();

        bool res = json::parse(descriptor, holder, 
                str.c_str(), str.size());

        if (res && m_data->widget)
        {
            m_data->widget->fromHolder(holder);

            event->acceptProposedAction();
        }
    } 
    catch(...) 
    {
        // parse exception: nothing to do
    }
}

void Drop::childEvent(QChildEvent * event)
{
    if (event->added())
    {
        install(event->child());
    }
}

void Drop::install(QObject * w)
{
    if (supportDrag(w))
    {
        dynamic_cast< QWidget *>(w)->setAcceptDrops(true);;
        w->installEventFilter(this);

        typedef QList< QWidget * > list_t;
        list_t ch = w->findChildren< QWidget * >();

        for (list_t::iterator it = ch.begin(); it != ch.end(); ++it) 
        {
            if (supportDrag(*it))
            {
                (*it)->setAcceptDrops(true);;
                (*it)->installEventFilter(this);
            }
        }
    }
}

void Drop::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->source() != m_data->qwidget 
            && event->mimeData()->hasFormat("text/plain"))
    {
        event->acceptProposedAction();
    }
}

