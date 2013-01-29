#include "ParameterModel.hpp"
#include <QtGui>

using namespace gsim::qt;

ParameterModel::ParameterModel(QObject *parent) : 
    QAbstractItemModel(parent), 
    m_checkable(false)
{
}

ParameterModel::~ParameterModel()
{
}

void ParameterModel::initialize(const QString& name, TypeDescriptor_ptr descriptor)
{
    m_root.reset(new DescriptorNode(descriptor));
    m_root->text = name.toStdString();
    m_root->check_for_initialized();
    reset();
}

int ParameterModel::columnCount(const QModelIndex&) const
{
    return 1;
}

bool ParameterModel::isCheckable() const
{
    return m_checkable;
}

void ParameterModel::setCheckable(bool checkable)
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

QVariant ParameterModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || !m_root)
        return QVariant();

    AbstractNode * node = static_cast< AbstractNode * >(
            index.internalPointer());

    if (role == Qt::DisplayRole || role == Qt::EditRole)
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
    else if (role == Qt::CheckStateRole)
    {
        DescriptorNode * dNode = 
            static_cast< DescriptorNode *>(node);
        
        dNode->check_for_initialized();

        if (isCheckable(dNode->descriptor))
            return (node->checked)? Qt::Checked: Qt::Unchecked;
    }

    return QVariant();
}

bool ParameterModel::setData(const QModelIndex & index, 
        const QVariant& value, int role)
{
    if (!index.isValid() || !m_root)
        return false;

    DescriptorNode * node = 
        static_cast< DescriptorNode * >(index.internalPointer());

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
            node->checked = check;

            emit dataChanged(index, index);

            if (check)
            {
                emit checked(m_root->descriptor, path);
            }
            else
            {
                emit unchecked(m_root->descriptor, path);
            }
        }

        return true;
    }

    return false;
}

Qt::ItemFlags ParameterModel::flags(const QModelIndex &index) const
{
    if (!index.isValid() || !m_root)
        return 0;
    
    Qt::ItemFlags f =  Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    DescriptorNode * node = static_cast< DescriptorNode * >(
        index.internalPointer());

    node->check_for_initialized();

    if (isCheckable(node->descriptor))
        f = f | Qt::ItemIsUserCheckable;

    return f;
}

QVariant ParameterModel::headerData(int section, 
        Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal 
            && role == Qt::DisplayRole)
    {
        switch (section)
        {
        case 0:
            return QString("Parameters");
        default:
            break;
        }
    }

    return QVariant();
}

QModelIndex ParameterModel::index(int row, int column, 
        const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();
    
    if (!parent.isValid())
    {
        return createIndex(row, column, 
                (void *) m_root.get());
    }

    DescriptorNode * dNode = 
        static_cast< DescriptorNode *>(parent.internalPointer());
    
    dNode->check_for_initialized();

    if (row < (int) dNode->children.size())
    {
        return createIndex(row, column, 
                (void *) dNode->children[row].get());
    }

    return QModelIndex();
}

QModelIndex ParameterModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    DescriptorNode * dNode = 
        static_cast< DescriptorNode * >(index.internalPointer());

    if (dNode)
    {
        dNode->check_for_initialized();

        // Message node
        if (!dNode->parent)
            return QModelIndex();
            
        return createIndex(dNode->index, 0, (void *) dNode->parent);
    }

    return QModelIndex();
}

int ParameterModel::rowCount(const QModelIndex &parent) const
{
    /**
     * Los hijos son de la columna cero.
     */
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        return 1; 

    DescriptorNode * node = 
        static_cast< DescriptorNode * >(parent.internalPointer());

    if (node)
    {
        node->check_for_initialized();

        return node->children.size();
    }

    return 0;
}

// Accessors
bool ParameterModel::isMessageNode(const QModelIndex& index) const
{
    if (!index.isValid())
        return false;

    AbstractNode * node = 
        static_cast< AbstractNode * >(index.internalPointer());

    DescriptorNode * dNode = 
        dynamic_cast< DescriptorNode * >(node);

    return dNode && !dNode->parent;
}

// Checkable
bool ParameterModel::isCheckable(TypeDescriptor_ptr reflective) const
{
    return m_checkable;
}

QModelIndex ParameterModel::toModelIndex(
        TypeDescriptor_ptr message,
        const QList< int >& path)
{
    if (!m_root || m_root->descriptor != message)
        return QModelIndex();

    QModelIndex idx = index(0, 0, QModelIndex());

    // No comprueba si existe ese camino. El resultado es incorrecto
    // para caminos inexistentes.
    // Simplemente va creando los índices sin comprobación.
    for (int ii = 0; ii < path.size(); ii++) 
    {
        idx = index(path[ii], 0, idx);
    }

    return idx;
}

void ParameterModel::check(TypeDescriptor_ptr message,
            const QList< int >& path)
{
    const QModelIndex idx = toModelIndex(message, path);
    setData(idx, Qt::Checked, Qt::CheckStateRole);
}

void ParameterModel::uncheck(TypeDescriptor_ptr message,
            const QList< int >& path)
{
    const QModelIndex idx = toModelIndex(message, path);
    setData(idx, Qt::Unchecked, Qt::CheckStateRole);
}

