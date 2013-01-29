#include "MessageModel.hpp"
#include <QtGui>
#include <iostream>
#include <limits>
#include <gsim/qt/model/Connection.hpp>

using namespace gsim::qt;

MessageModel::MessageModel(Direction direction, QObject *parent) : 
    QAbstractItemModel(parent), 
    m_direction(direction),
    m_maxLevel(std::numeric_limits< int >::max()),
    m_checkable(false)
{
}

MessageModel::~MessageModel()
{
}

void MessageModel::registerInstance(Connection_ptr connection)
{
    GroupNode_ptr node(new GroupNode(connection, 
                ((m_direction == kIn)? 
                    connection->descriptor()->in():
                    connection->descriptor()->out())));

    m_nodes.push_back(node);
        
    reset();
}

void MessageModel::unregisterInstance(Connection_ptr connection)
{
    int i = 0;
    for (Nodes_t::iterator it = m_nodes.begin();
            it != m_nodes.end(); ++it, i++) 
    {
        if ((*it)->instance == connection)
        {
            m_nodes.erase(it);

            reset();
            break;
        }
    }
}

bool MessageModel::displayMessages() const
{
    return (m_maxLevel > 0);
}

bool MessageModel::displayParameters() const
{
    return (m_maxLevel > 1);
}

int MessageModel::maxLevel() const
{
    return m_maxLevel;
}

void MessageModel::setDisplayMessages(bool value)
{
    m_maxLevel = (value)? 1: 0;

    reset();
}

void MessageModel::setDisplayParameters(bool value)
{
    m_maxLevel = (value)? std::numeric_limits< int >::max(): 1;

    reset();
}

void MessageModel::setMaxLevel(int value)
{
    m_maxLevel = value;
}

int MessageModel::columnCount(const QModelIndex&) const
{
    return 1;
}

bool MessageModel::isCheckable() const
{
    return m_checkable;
}

void MessageModel::setCheckable(bool checkable)
{
    m_checkable = checkable;
    reset();
}

namespace  
{

QString getNodeName(DescriptorNode const * node)
{
    using namespace gsim::core;
    DescriptorNode const * parent = node->parent;

    if (parent)
    {
        TypeDescriptor_ptr reflective = parent->descriptor;

        if (reflective->is_repeated())
        {
            return QString("[%1]").arg(node->index);
        }
        else if (reflective->get_type() == TYPE_STRUCT)
        {
            return reflective->get_child_name(node->index);
        }
    }

    return "Error!";
}

} // namespace

QVariant MessageModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    AbstractNode * node = static_cast< AbstractNode * >(
            index.internalPointer());

    GroupNode * iNode = 
        dynamic_cast< GroupNode *>(node);

    if (role == Qt::ToolTipRole)
    {
        if (iNode)
        {
            return QString("Connection: %1")
                .arg(iNode->instance->name());
        }
        else
        {
            DescriptorNode * dNode = 
                static_cast< DescriptorNode *>(node);
            
            dNode->check_for_initialized();

            // Message node
            if (!dNode->parent)
            {
                // Message name
                return QString("Connection: %1, Message: %2")
                    .arg(dNode->instance->instance->name())
                    .arg(dNode->text.c_str());
            }
            else
            {
                return getNodeName(dNode);
            }
        }
    }
    else if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (iNode)
        {
            iNode->check_for_initialized();

            return iNode->instance->name();
        }
        else
        {
            DescriptorNode * dNode = 
                static_cast< DescriptorNode *>(node);
            
            dNode->check_for_initialized();

            // Message node
            if (!dNode->parent)
            {
                // Message name
                return QString(dNode->text.c_str());
            }
            else
            {
                return getNodeName(dNode);
            }
        }
    }
    else if (!iNode && role == Qt::CheckStateRole)
    {
        DescriptorNode * dNode = 
            static_cast< DescriptorNode *>(node);
        
        dNode->check_for_initialized();

        if (isCheckable(dNode->descriptor))
            return (node->checked)? Qt::Checked: Qt::Unchecked;
    }

    return QVariant();
}

bool MessageModel::setData(const QModelIndex & index, 
        const QVariant& value, int role)
{
    if (!index.isValid())
        return false;

    AbstractNode * aNode = 
        static_cast< AbstractNode * >(index.internalPointer());
    DescriptorNode * node = 
        dynamic_cast< DescriptorNode * >(aNode);

    if (role == Qt::CheckStateRole && node && isCheckable(node->descriptor))
    {
        QList< int > path;
        node->check_for_initialized();

        const DescriptorNode * cnode = node;

        // Calculate path
        while(cnode && cnode->parent)
        {
            path.push_front(cnode->index);
            cnode = cnode->parent;
        }
        
        Qt::CheckState state = 
            static_cast<Qt::CheckState>(value.toUInt());

        bool check = (state == Qt::Checked)? true: false;
        bool old = node->checked;

        if (check != old)
        {
            TypeDescriptor_ptr message = cnode->descriptor;
            Connection_ptr connection (cnode->instance->instance);
            node->checked = check;

            emit dataChanged(index, index);

            if (check)
            {
                emit checked(connection, message, path);
            }
            else
            {
                emit unchecked(connection, message, path);
            }
        }

        return true;
    }

    return false;
}

Qt::ItemFlags MessageModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    
    Qt::ItemFlags f =  Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if (!isGroupNode(index))
    {
        DescriptorNode * node = static_cast< DescriptorNode * >(
                index.internalPointer());

        node->check_for_initialized();

        if (isCheckable(node->descriptor))
            f = f | Qt::ItemIsUserCheckable;
    }

    return f;
}

QVariant MessageModel::headerData(int section, 
        Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal 
            && role == Qt::DisplayRole)
    {
        switch (section)
        {
        case 0:
            return QString("Connections");
        default:
            break;
        }
    }

    return QVariant();
}

QModelIndex MessageModel::index(int row, int column, 
        const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    if (!parent.isValid())
    {
        return createIndex(row, column, 
                (void *) m_nodes.at(row).get());
    }

    AbstractNode * node = static_cast< AbstractNode * >(
            parent.internalPointer());

    GroupNode * iNode = 
        dynamic_cast< GroupNode *>(node);

    if (iNode)
    {
        iNode->check_for_initialized();

        if (row < (int) iNode->children.size())
        {
            return createIndex(row, column, 
                    (void *) iNode->children[row].get());
        }
    }
    else
    {
        DescriptorNode * dNode = 
            static_cast< DescriptorNode *>(node);
        
        dNode->check_for_initialized();

        if (row < (int) dNode->children.size())
        {
            return createIndex(row, column, 
                    (void *) dNode->children[row].get());
        }
    }

    return QModelIndex();
}

QModelIndex MessageModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    AbstractNode * node = static_cast< AbstractNode * >(
            index.internalPointer());

    if (!node)
        return QModelIndex();

    DescriptorNode * dNode = 
        dynamic_cast< DescriptorNode *>(node);

    if (dNode)
    {
        dNode->check_for_initialized();

        // Message node
        if (!dNode->parent)
            return createIndex(dNode->index, 0, (void *) dNode->instance);
            
        return createIndex(dNode->index, 0, (void *) dNode->parent);
    }

    return QModelIndex();
}

int MessageModel::rowCount(const QModelIndex &parent) const
{
    /**
     * Los hijos son de la columna cero.
     */
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        return m_nodes.size(); 

    if (!parent.parent().isValid())
    {
        GroupNode * node = 
            static_cast< GroupNode * >(parent.internalPointer());

        if (node && displayMessages())
        {
            node->check_for_initialized();

            return node->children.size();
        }
    }
    else 
    {
        DescriptorNode * node = 
            static_cast< DescriptorNode * >(parent.internalPointer());

        if (node && displayParameters())
        {
            node->check_for_initialized();

            return node->children.size();
        }
    }

    return 0;
}

// Accessors
bool MessageModel::isMessageNode(const QModelIndex& index) const
{
    if (!index.isValid())
        return false;

    AbstractNode * node = 
        static_cast< AbstractNode * >(index.internalPointer());

    DescriptorNode * dNode = 
        dynamic_cast< DescriptorNode * >(node);

    return dNode && !dNode->parent;
}

bool MessageModel::isGroupNode(const QModelIndex& index) const
{
    if (!index.isValid())
        return false;

    return !index.parent().isValid();
}

/*
MessageDescriptor_ptr MessageModel::getMessage(
        const QString& instance,
        const QString& operation) const
{
    Connection_ptr obj = m_instances.find(instance);

    if (obj)
    {
        const std::string str (operation.toStdString());
        return obj->interface()->get_reflective_by_name(str.c_str());
    }

    return NULL;
}
*/

Connection_ptr MessageModel::getInstance(const QModelIndex& index) const
{
    if (!index.isValid())
        return Connection_ptr();

    if (!index.parent().isValid())
    {
        GroupNode * node = 
            static_cast< GroupNode * >(index.internalPointer());

        return node->instance;
    }
    else 
    {
        DescriptorNode * node = 
            static_cast< DescriptorNode * >(index.internalPointer());

        return node->instance->instance;
    }

    return Connection_ptr();
}


Connection_ptr MessageModel::getInstance(const QString& name) const
{
    for (Nodes_t::const_iterator it = m_nodes.begin();
            it != m_nodes.end(); ++it) 
    {
        if ((*it)->instance->name() == name)
        {
            return (*it)->instance;
        }
    }

    return Connection_ptr();
}

// Checkable
bool MessageModel::isCheckable(TypeDescriptor_ptr reflective) const
{
    return m_checkable;
}

QModelIndex MessageModel::toModelIndex(Connection_ptr connection, 
            TypeDescriptor_ptr message,
            const QList< int >& path)
{
    int i = 0;

    // Busca el nodo del a conexión
    for (Nodes_t::iterator it = m_nodes.begin();
            it != m_nodes.end(); ++it, i++) 
    {
        const GroupNode_ptr node = *it;

        if (node->instance == connection)
        {
            QModelIndex idx = index(i, 0, QModelIndex());

            // Busca el nodo del mensaje
            for (int j = 0; j < (int) node->children.size(); j++) 
            {
                // El nodo del mensaje se identifica por su descriptor
                if (node->children[j]->descriptor == message)
                {
                    idx = index(j, 0, idx);

                    // No comprueba si existe ese camino. El resultado es incorrecto
                    // para caminos inexistentes.
                    // Simplemente va creando los índices sin comprobación.
                    for (int ii = 0; ii < path.size(); ii++) 
                    {
                        idx = index(path[ii], 0, idx);
                    }

                    return idx;
                }
            }

            return QModelIndex();
        }
    }

    return QModelIndex();
}

void MessageModel::check(Connection_ptr connection, 
            TypeDescriptor_ptr message,
            const QList< int >& path)
{
    const QModelIndex idx = toModelIndex(connection, message, path);
    setData(idx, Qt::Checked, Qt::CheckStateRole);
}

void MessageModel::uncheck(Connection_ptr connection, 
            TypeDescriptor_ptr message,
            const QList< int >& path)
{
    const QModelIndex idx = toModelIndex(connection, message, path);
    setData(idx, Qt::Unchecked, Qt::CheckStateRole);
}

