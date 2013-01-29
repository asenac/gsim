// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * LogModel.cpp
 *
 * CORBASIM is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * CORBASIM is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "LogModel.hpp"
#include <QStyle>
#include <QApplication>

#include <gsim/qt/reflective/qvariant.hpp>

#include <gsim/qt/constants.hpp>
#include <gsim/core/descriptor_utils.hpp>

// #include <gsim/qt/item/FilterModel.hpp>

#include <gsim/core/descriptor.hpp>
#include <gsim/core/holder.hpp>
#include <gsim/core/event.hpp>

#include <gsim/json/reflective.hpp>

#include <gsim/qt/model/Connection.hpp>

#include <iostream>
#include <algorithm>

using namespace gsim::qt;

class LogModel::Data
{
public:

    Data(QObject * this_) : 
        maxEntries(GSIM_LOG_DEFAULT_MAX_SIZE),
        editable(false),
        pendingTimer(this_)
    {}

    int maxEntries;
    bool editable;

    QIcon inputIcon;
    QIcon outputIcon;

    QList< LogEntry > entries;
    QList< LogEntry > pendingEntries;
    QList< MetaNode_ptr > nodes;

    QTime delayDiff;

    QTimer pendingTimer;
};

LogModel::LogModel(QObject * parent) :
    QAbstractItemModel(parent), m_data(new Data(this))
{
    m_data->inputIcon = qApp->style()->standardIcon(QStyle::SP_ArrowRight);
    m_data->outputIcon = qApp->style()->standardIcon(QStyle::SP_ArrowLeft);

#if QT_VERSION >= 0x0401700
    m_data->entries.reserve(2 * m_data->maxEntries);
#endif

    // Timer
    m_data->pendingTimer.setSingleShot(true);
    m_data->pendingTimer.setInterval(GSIM_LOG_TIMER_INTERVAL);
    connect(&m_data->pendingTimer, SIGNAL(timeout()),
            this, SLOT(processPendingEntries()));

    setSupportedDragActions(Qt::CopyAction);
}

LogModel::~LogModel()
{
    delete m_data;
}

QMimeData *	LogModel::mimeData(const QModelIndexList& indexes) const
{
    if (indexes.size() == 2) // both columns
    {
        int pos = indexToPosition(indexes.at(0));

        const LogEntry& entry = m_data->entries.at(pos);

        core::holder holder = entry.msg->holder();

        if (core::utils::calculate_size(holder) < GSIM_DRAG_MAX_SIZE)
        {
            std::ostringstream oss;
            json::write(oss, holder.get_type_descriptor(), holder);

            QMimeData *mimeData = new QMimeData;
            mimeData->setText(oss.str().c_str());

            return mimeData;
        }
    }

    return NULL;
}

int LogModel::indexToPosition(const QModelIndex& index) const
{
    if (!index.isValid())
        return -1;

    QModelIndex parent = index;

    while (parent.parent().isValid())
        parent = parent.parent();

    return parent.row();
}

int LogModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 2;
}

const LogModel::LogEntry * LogModel::getEntry(const QModelIndex& index) const
{
    QModelIndex idx = index;

    while(idx.parent().isValid()) idx = idx.parent();

    if (idx.isValid() && idx.row() < m_data->entries.size())
    {
        return &(m_data->entries.at(idx.row()));
    }

    return NULL;
}

namespace 
{

QString getNodeName(MetaNode const * node)
{
    if (node->parent && node->parent->descriptor->is_repeated())
    {
        return QString("[%1]").arg(node->index);
    }
    else if (node->parent && node->parent->descriptor->get_type() ==
            gsim::core::TYPE_STRUCT)
    {
        return node->parent->descriptor->get_child_name(node->index);
    }

    return "Error!";
}

} // namespace 


QVariant LogModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // Background
    if (role == Qt::BackgroundRole)
    {
        QModelIndex parent = index;
        while(parent.parent().isValid())
        {
            parent = parent.parent();
        }

        // std::cout << parent.row() << std::endl;
        const LogEntry& entry = m_data->entries.at(parent.row());

        // return QVariant();
        return entry.color;
    }
    else if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        MetaNode * node = 
            static_cast< MetaNode * >(index.internalPointer());
        node->check_for_initialized();

        // First level item
        if (!index.parent().isValid())
        {
            // Value
            if (index.column())
                return m_data->entries[index.row()].dateTime;

            return m_data->entries[index.row()].text;
        }
        else
        {
            // Value
            if (index.column())
            {
                int col = index.column() - 1;

                if (col < (int) node->brothers.size() 
                        && node->brothers[col])
                    return toQVariant(node->descriptor, 
                            node->brothers[col]->holder);

                return QVariant();
            }

            // Name
            return getNodeName(node);
        }
    }
#if 1
    else if (role == Qt::ToolTipRole)
    {
        // Alternativa 1: Tool tip en JSON
        core::holder holder;

        if (!index.parent().isValid())
        {
            holder = m_data->entries[index.row()].msg->holder();
        }
        else
        {
            MetaNode * node = 
                static_cast< MetaNode * >(index.internalPointer());
            node->check_for_initialized();

            if (node->brothers.size())
                holder = node->brothers[0]->holder;
        }

        if (holder.is_valid() && 
                core::utils::calculate_size(holder) < GSIM_DRAG_MAX_SIZE)
        {
            std::ostringstream oss;
            json::write(oss, holder.get_type_descriptor(), holder, true);

            return oss.str().c_str();
        }
    }
#endif
    else if (!index.parent().isValid() && index.column() == 0
            && role == Qt::DecorationRole)
    {
        const LogEntry& entry = m_data->entries.at(index.row());
        return *(entry.icon);
    }

    return QVariant();
}

bool LogModel::setData(const QModelIndex & index, 
        const QVariant& value, int role)
{
    MetaNode * node = static_cast< MetaNode * >(index.internalPointer());
    
    if (!node) return false;

    node->check_for_initialized();

    const int col = index.column() - 1;

    if (col < 0 || col >= (int) node->brothers.size() 
            || !node->brothers[col])
        return false;

    bool res = fromQVariant(node->descriptor, 
            node->brothers[col]->holder, value);

    if (res)
    {
        emit dataChanged(index, index);
    }

    return res;
}

Qt::ItemFlags LogModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    // Value is editable by default
    if (index.column() && m_data->editable)
        return Qt::ItemIsEnabled 
            | Qt::ItemIsSelectable 
            | Qt::ItemIsEditable
            | Qt::ItemIsDragEnabled
            ;

    return Qt::ItemIsEnabled 
        | Qt::ItemIsSelectable 
        | Qt::ItemIsDragEnabled;
}

QVariant LogModel::headerData(int section, 
        Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch (section)
        {
        case 0:
            return QString("Message");
        case 1:
            return QString("Value");
        default:
            break;
        }
    }

    return QVariant();
}

QModelIndex LogModel::index(int row, int column, 
        const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    if (!parent.isValid())
    {
        return createIndex(row, column, 
                (void *) m_data->nodes[row].get());
    }

    MetaNode * node = static_cast< MetaNode * >(
            parent.internalPointer());
    node->check_for_initialized();

    if (row < (int) node->children.size())
    {
        return createIndex(row, column, 
                (void *) node->children[row].get());
    }

    return QModelIndex();
}

QModelIndex LogModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    MetaNode * node = static_cast< MetaNode * >(
            index.internalPointer());

    if (!node || !node->parent)
        return QModelIndex();

    // parent is first level item
    if (!node->parent->parent)
    {
        // index could be changed
        int row = 0;
        for (; row < m_data->nodes.size(); row++) 
            if (m_data->nodes.at(row).get() == node->parent)
                return createIndex(row, 0, (void *) node->parent);

        return QModelIndex();
    }

    return createIndex(node->index, 0, (void *) node->parent);
}

int LogModel::rowCount(const QModelIndex &parent) const
{
    /**
     * Los hijos son de la columna cero.
     */
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        return m_data->entries.size();

    MetaNode * node = static_cast< MetaNode * >(
            parent.internalPointer());
    node->check_for_initialized();

    return node->children.size();
}

void LogModel::clearLog()
{
    beginResetModel();
    reset();
    resetInternalData();
    endResetModel();
}

void LogModel::resetInternalData()
{
    m_data->entries.clear();
    m_data->pendingEntries.clear();
    m_data->nodes.clear();
}

int LogModel::maxEntries() const
{
    return m_data->maxEntries;
}

void LogModel::setMaxEntries(int max)
{
    m_data->maxEntries = max;
#if QT_VERSION >= 0x0401700
    m_data->entries.reserve(2 * m_data->maxEntries);
#endif
    
    // Aplica el nuevo tamaño
    removeEntries(0);
}

bool LogModel::isEditable() const
{
    return m_data->editable;
}

void LogModel::setEditable(bool editable)
{
    m_data->editable = editable;

    reset();
}

void LogModel::inputMessage(Connection_ptr con, 
        Message_ptr msg)
{
    append(con, msg, true);
}

void LogModel::outputMessage(Connection_ptr con, 
        Message_ptr msg)
{
    append(con, msg, false);
}

void LogModel::removeEntries(int requiredFreeEntries)
{
    // Deja espacio
    int nRowsToBeRemoved = 
        m_data->entries.size() - m_data->maxEntries + requiredFreeEntries;

    // El número a eliminar no puede ser superior al de entradas
    // existentes
    nRowsToBeRemoved = std::min(m_data->entries.size(), nRowsToBeRemoved);

    if (nRowsToBeRemoved > 0)
    {
        beginRemoveRows(QModelIndex(), 0, nRowsToBeRemoved - 1);

        for (int i = 0; i < nRowsToBeRemoved; i++) 
        {
            // Elimina la primera
            m_data->entries.pop_front();
            m_data->nodes.pop_front();
        }

        endRemoveRows();
    }
}

void LogModel::processPendingEntries()
{
    if (!m_data->pendingEntries.isEmpty())
    {
        int size = m_data->pendingEntries.size();

        size = std::min(size, m_data->maxEntries);

        removeEntries(size);

        // No inserta más de las maximas permitidas
        if (m_data->pendingEntries.size() > size)
        {
            const int diff = m_data->pendingEntries.size() - size;

            // TODO No me gusta esta solución: Eliminar unas cuantas 
            // para despùés hacer 'clear'
            // Debería ser un paso de directo: Insertar desde
            // begin + diff hasta end
            m_data->pendingEntries.erase(m_data->pendingEntries.begin(),
                    m_data->pendingEntries.begin() + diff);
        }

        beginInsertRows(QModelIndex(), m_data->entries.size(), 
                m_data->entries.size() + size - 1);

        // Hace accesibles las entradas pendientes
        m_data->entries.append(m_data->pendingEntries);
        m_data->pendingEntries.clear();
        
        m_data->delayDiff.start();

        endInsertRows();
    }
}

void LogModel::append(Connection_ptr con, Message_ptr msg, bool is_in)
{
    LogEntry entry;
    fillLogEntry(con, msg, is_in, entry);

    // Se intenta que haya dos repintados como máximo cada medio segundo:
    // el de eliminar y el de añadir
    if (m_data->delayDiff.elapsed() < GSIM_LOG_REQUIRED_ELAPSED)
    {
        m_data->pendingEntries.push_back(entry);

        if (!m_data->pendingTimer.isActive())
            m_data->pendingTimer.start();
    }
    else
    {
        // Aquí se producen 4 repintados en el peor de los casos.

        // Asegura que se inserta en orden
        // Nunca se ha de insertar fuera de ornde
        processPendingEntries();

        removeEntries(1);

        beginInsertRows(QModelIndex(), m_data->entries.size(), 
                m_data->entries.size());

        m_data->entries.push_back(entry);

        m_data->delayDiff.start();

        endInsertRows();
    }
}

void LogModel::fillLogEntry(Connection_ptr con, Message_ptr msg, 
        bool is_in,
        LogEntry& entry)
{
    entry.dateTime = QDateTime::currentDateTime();
    entry.connection = con;

    core::holder hold = msg->holder();
    TypeDescriptor_ptr descriptor = hold.get_type_descriptor();

    MetaNode_ptr metaNode(new MetaNode(descriptor));
    
    // Message
    Node_ptr node(new Node(descriptor, hold));
    metaNode->brothers.push_back(node);
    
    m_data->nodes.push_back(metaNode);
    
    entry.is_in_entry = is_in;
    entry.msg = msg;

    if (is_in)
    {
        entry.text = QString("Incoming message in ") + con->name() + " - " 
            + msg->name().c_str();
        entry.icon = &m_data->inputIcon;
        
        entry.color = QColor(Qt::green);
    }
    else
    {
        entry.text = QString("Outgoing message in ") + con->name() + " - " 
            + msg->name().c_str();
        entry.icon = &m_data->outputIcon;

        entry.color = QColor(Qt::yellow);
    }
}

const LogModel::LogEntry& LogModel::getLogEntry(int row) const 
{ 
    return m_data->entries.at(row); 
}

/*
FilteredLogModel::FilteredLogModel(QObject * parent) : 
    QSortFilterProxyModel(parent), m_filter(NULL)
{
}

FilteredLogModel::~FilteredLogModel()
{
}

bool FilteredLogModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    if (sourceParent.isValid()) return true;

    if (!m_filter) return false;

    LogModel const * model = dynamic_cast< LogModel const * >(sourceModel());

    if (model)
    {
        const LogModel::LogEntry& entry(model->getLogEntry(sourceRow));

        return m_filter->visibleOperation(entry.object->id(), entry.reflective->get_tag());
    }

    return false;
}

void FilteredLogModel::setFilterModel(FilterModel * filter)
{
    if (m_filter)
    {
        QObject::disconnect(m_filter, SIGNAL(filterChanged()), this, SLOT(resetModel()));
    }

    m_filter = filter;

    if (m_filter)
    {
        QObject::connect(m_filter, SIGNAL(filterChanged()), this, SLOT(resetModel()));
    }
}

void FilteredLogModel::resetModel()
{
    invalidateFilter();
}
*/
