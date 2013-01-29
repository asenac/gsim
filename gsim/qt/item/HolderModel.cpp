// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * HolderModel.cpp
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

#include "HolderModel.hpp"
#include <QMimeData>

#include <gsim/qt/reflective/qvariant.hpp>
#include <gsim/qt/constants.hpp>
#include <gsim/core/descriptor_utils.hpp>
#include <gsim/core/descriptor.hpp>
#include <gsim/core/holder.hpp>
#include <gsim/json/reflective.hpp>

#include <iostream>
#include <algorithm>

using namespace gsim::qt;

class HolderModel::Data
{
public:

    Data(QObject * this_) : 
        editable(false)
    {}

    bool editable;

    // Root node
    Node_ptr node;
};

HolderModel::HolderModel(TypeDescriptor_ptr descriptor, Holder holder, 
        QObject * parent) :
    QAbstractItemModel(parent), m_data(new Data(this))
{
    m_data->node = Node_ptr(new Node(descriptor, holder));
    m_data->node->check_for_initialized();

    setSupportedDragActions(Qt::CopyAction);
}

HolderModel::~HolderModel()
{
    delete m_data;
}

QMimeData *	HolderModel::mimeData(const QModelIndexList& indexes) const
{
    if (indexes.size() == 2) // both columns
    {
        core::holder& holder = m_data->node->holder;

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

int HolderModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 2;
}

namespace 
{

QString getNodeName(Node const * node)
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


QVariant HolderModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // Background
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        Node * node = 
            static_cast< Node * >(index.internalPointer());
        node->check_for_initialized();

        // Value
        if (index.column())
        {
            return toQVariant(node->descriptor, node->holder);
        }

        // Name
        return getNodeName(node);
    }

    return QVariant();
}

bool HolderModel::setData(const QModelIndex & index, 
        const QVariant& value, int role)
{
    Node * node = static_cast< Node * >(index.internalPointer());
    
    if (!node) return false;

    node->check_for_initialized();

    const int col = index.column() - 1;

    if (col < 0)
        return false;

    bool res = fromQVariant(node->descriptor, 
            node->holder, value);

    if (res)
    {
        emit dataChanged(index, index);
    }

    return res;
}

Qt::ItemFlags HolderModel::flags(const QModelIndex &index) const
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

QVariant HolderModel::headerData(int section, 
        Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch (section)
        {
        case 0:
            return QString("Field");
        case 1:
            return QString("Value");
        default:
            break;
        }
    }

    return QVariant();
}

QModelIndex HolderModel::index(int row, int column, 
        const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    if (!parent.isValid())
    {
        return createIndex(row, column, 
                (void *) m_data->node->children[row].get());
    }

    Node * node = static_cast< Node * >(
            parent.internalPointer());
    node->check_for_initialized();

    if (row < (int) node->children.size())
    {
        return createIndex(row, column, 
                (void *) node->children[row].get());
    }

    return QModelIndex();
}

QModelIndex HolderModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    Node * node = static_cast< Node * >(
            index.internalPointer());

    if (!node || node->parent == m_data->node.get())
        return QModelIndex();

    return createIndex(node->index, 0, (void *) node->parent);
}

int HolderModel::rowCount(const QModelIndex &parent) const
{
    /**
     * Los hijos son de la columna cero.
     */
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        return m_data->node->children.size();

    Node * node = static_cast< Node * >(
            parent.internalPointer());
    node->check_for_initialized();

    return node->children.size();
}

void HolderModel::resetModel()
{
    beginResetModel();
    reset();
    resetInternalData();
    endResetModel();
}

void HolderModel::resetInternalData()
{
    m_data->node->reset();
    m_data->node->check_for_initialized();
}

bool HolderModel::isEditable() const
{
    return m_data->editable;
}

void HolderModel::setEditable(bool editable)
{
    m_data->editable = editable;

    reset();
}

