// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * DragAndDrop.hpp
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

#ifndef GSIM_QT_DRAGANDDROP_HPP
#define GSIM_QT_DRAGANDDROP_HPP

#include <QtGui>

namespace gsim 
{
namespace qt 
{

class Drag : public QObject
{
    Q_OBJECT
public:

    Drag(QWidget * refWidget, QObject * parent = 0);
    ~Drag();

    void dragLeaveEvent(QDragLeaveEvent *event);

    void mousePressEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);

protected:
    
    void install(QObject * w);

    bool eventFilter(QObject * obj, QEvent * event);

    void childEvent(QChildEvent * event);

    class Data;
    Data * m_data;
};

class Drop : public QObject
{
    Q_OBJECT
public:

    Drop(QWidget * refWidget, QObject * parent = 0);
    ~Drop();

    void dropEvent(QDropEvent *event);

    void dragEnterEvent(QDragEnterEvent *event);

protected:

    void install(QObject * w);

    bool eventFilter(QObject * obj, QEvent * event);
    
    void childEvent(QChildEvent * event);

    class Data;
    Data * m_data;
};


} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_DRAGANDDROP_HPP */

