// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * DetailedLog.hpp
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

#ifndef GSIM_QT_DETAILEDLOG_HPP
#define GSIM_QT_DETAILEDLOG_HPP

#include <QtGui>
#include <gsim/qt/types.hpp>
#include <gsim/qt/item/LogModel.hpp>

namespace gsim 
{
namespace qt 
{

class LogModel;
class DetailedLogController;
class DetailedLogItem;

class DetailedLogItem : public QDialog
{
    Q_OBJECT
public:
    DetailedLogItem(
            DetailedLogController * controller,
            TypeDescriptor_ptr type, 
            QWidget * parent);
    ~DetailedLogItem();

public slots:

    void setValue(const LogModel::LogEntry * entry);

    void save();

protected:
    class Data;
    Data * m_data;
};

class DetailedLogController : public QObject
{
    Q_OBJECT
public:

    DetailedLogController(QWidget * parent);
    ~DetailedLogController();

    void setLogModel(LogModel * logModel);

public slots:

    void showDetails(const QModelIndex&);

protected:

    class Data;
    Data * m_data;
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_DETAILEDLOG_HPP */

