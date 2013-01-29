// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * StatusTool.hpp
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

#ifndef GSIM_QT_STATUSTOOL_HPP
#define GSIM_QT_STATUSTOOL_HPP

#include <gsim/qt/export.hpp>
#include <gsim/qt/types.hpp>
#include <QtGui>

namespace gsim 
{
namespace qt 
{

class StatusItem;
class StatusTool;

class GSIM_QT_DECLSPEC StatusTool : public QWidget
{
    Q_OBJECT
public:
    StatusTool(Connection_ptr connection,
          QWidget * parent = 0);
    ~StatusTool();

public slots:

    void apply();
    void copyCurrent();
    void sendCurrent();
    void setCurrentStatus(const QString& name, Holder currentStatus);

    void clearProposed();

protected slots:

    void selectedStatus(const QModelIndex&);

signals:

    void updateStatus(const QString& name, Holder newStatus);

    void sendCurrent(const QString&);

protected:
    
    class Data;
    Data * m_data;
};

class GSIM_QT_DECLSPEC StatusItem : public QObject
{
    Q_OBJECT
public:
    StatusItem(const QString& name,
            Holder currentStatus,
            QWidget * parent = 0);
    ~StatusItem();

    QWidget * proposedWidget() const;
    QWidget * currentWidget() const;
    
    Holder proposedStatus();

public slots:

    void clearProposed();
    void copyCurrent();
    void setCurrentStatus(Holder currentStatus);

protected:

    class Data;
    Data * m_data;
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_STATUSTOOL_HPP */

