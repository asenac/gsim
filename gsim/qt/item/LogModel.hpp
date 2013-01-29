// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * LogModel.hpp
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

#ifndef GSIM_QT_NEWLOGMODEL_HPP
#define GSIM_QT_NEWLOGMODEL_HPP

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QDateTime>
#include <QIcon>

#include <gsim/qt/export.hpp>
#include <gsim/qt/types.hpp>
#include <gsim/qt/item/ModelNode.hpp>

namespace gsim 
{
namespace qt
{

// class FilterModel;
// class FilteredLogModel;

class GSIM_QT_DECLSPEC LogModel : 
    public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(int maxEntries READ maxEntries WRITE setMaxEntries)
    Q_PROPERTY(bool editable READ isEditable WRITE setEditable)
public:
    LogModel(QObject * parent = 0);
    virtual ~LogModel();

    QVariant data(const QModelIndex& index, 
            int role = Qt::DisplayRole) const; 
    bool setData(const QModelIndex & index, 
            const QVariant& value, int role = Qt::EditRole);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    
    QMimeData *	mimeData(const QModelIndexList& indexes) const;

    struct LogEntry
    {
        bool is_in_entry;
        QString text;
        Connection_ptr connection;
        Message_ptr msg;
        QDateTime dateTime;
        QIcon * icon;
        QColor color;
        bool exception;
    };

    const LogEntry * getEntry(const QModelIndex& index) const;

public slots:

    void clearLog();

    int maxEntries() const;
    void setMaxEntries(int max);

    bool isEditable() const;
    void setEditable(bool editable);

    void inputMessage(Connection_ptr con, 
            Message_ptr msg);
    void outputMessage(Connection_ptr con, 
            Message_ptr msg);

private slots:

    void resetInternalData();

    void processPendingEntries();

protected:

    void append(Connection_ptr con, Message_ptr msg, bool is_in);

    const LogEntry& getLogEntry(int row) const;

    void fillLogEntry(Connection_ptr con, Message_ptr msg, bool is_in,
            LogEntry& entry);

    void removeEntries(int requiredFreeEntries);

    int indexToPosition(const QModelIndex& index) const;

    class Data;
    Data * m_data;

    // friend class FilteredLogModel;
};

/*
class GSIM_QT_DECLSPEC FilteredLogModel : 
    public QSortFilterProxyModel
{
    Q_OBJECT
public:

    FilteredLogModel(QObject * parent = 0);
    virtual ~FilteredLogModel();

    void setFilterModel(FilterModel * filter);

protected slots:

    void resetModel();

protected:

    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const;

    FilterModel * m_filter;
};
*/

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_NEWLOGMODEL_HPP */

