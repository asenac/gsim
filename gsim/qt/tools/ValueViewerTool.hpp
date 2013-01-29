// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * ValueViewerTool.hpp
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

#ifndef GSIM_QT_VALUEVIEWERTOOL_HPP
#define GSIM_QT_VALUEVIEWERTOOL_HPP

#include <QtGui>
#include <gsim/qt/export.hpp>
#include <gsim/qt/tools/AbstractInputTool.hpp>
#include <gsim/qt/proc/MessageProcessor.hpp>

namespace gsim 
{
namespace qt 
{

class GSIM_QT_DECLSPEC ValueViewer : public AbstractInputItem
{
    Q_OBJECT
public:

    ValueViewer(Connection_ptr connection_,
            TypeDescriptor_ptr message_,
            const QList< int >& path_, 
            QWidget * parent = 0);
    virtual ~ValueViewer();

    void save(QVariant& settings);
    void load(const QVariant& settings);

    void start();

public slots:

    void appendValue(Message_ptr, Holder);

protected:

    QDateTimeEdit * m_date;
    QWidget * m_widget;
    QObject * m_drag;
};

class GSIM_QT_DECLSPEC ValueViewerTool : public AbstractInputTool
{
    Q_OBJECT
public:

    ValueViewerTool(QWidget * parent = 0);
    virtual ~ValueViewerTool();

protected:

    AbstractInputItem * createItem(
            Connection_ptr connection, 
            TypeDescriptor_ptr reflective,
            const QList< int >& path);

};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_VALUEVIEWERTOOL_HPP */
