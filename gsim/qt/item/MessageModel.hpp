#ifndef GSIM_QT_INSTANCEMODEL_HPP
#define GSIM_QT_INSTANCEMODEL_HPP

#include <QList>
#include <QIcon>
#include <QAbstractItemModel>
#include <gsim/qt/export.hpp>
#include <gsim/qt/types.hpp>
#include <gsim/qt/item/ModelNode.hpp>

namespace gsim
{
namespace qt
{

class GSIM_QT_DECLSPEC MessageModel : 
    public QAbstractItemModel
{
    Q_OBJECT
/*
    Q_PROPERTY(bool checkable 
            READ isCheckable
            WRITE setCheckable)
 */
    Q_PROPERTY(bool displayMessages 
            READ displayMessages
            WRITE setDisplayMessages)
    Q_PROPERTY(bool displayParameters 
            READ displayParameters
            WRITE setDisplayParameters)
    Q_PROPERTY(bool maxLevel 
            READ maxLevel
            WRITE setMaxLevel)
    Q_PROPERTY(bool checkable 
            READ isCheckable
            WRITE setCheckable)
public:

    enum Direction
    {
        kIn, kOut
    };

    MessageModel(Direction direction, QObject *parent = 0);
    virtual ~MessageModel();

    QVariant data(const QModelIndex& index, 
            int role) const; 
    bool setData(const QModelIndex & index, 
            const QVariant& value, int role);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;

    // void save(QVariant& settings);
    // void load(const QVariant& settings);

    bool displayMessages() const;
    bool displayParameters() const;
    int maxLevel() const;

    bool isCheckable() const;
    void setCheckable(bool checkable);

    bool isMessageNode(const QModelIndex& index) const;
    bool isGroupNode(const QModelIndex& index) const;
/*
    MessageDescriptor_ptr getMessage(const QString& instance,
            const QString& operation) const;
*/
    Connection_ptr getInstance(const QModelIndex&) const;
    Connection_ptr getInstance(const QString& name) const;

public slots:

    void registerInstance(Connection_ptr connection);
    void unregisterInstance(Connection_ptr connection);

    void setDisplayMessages(bool value);
    void setDisplayParameters(bool value);
    void setMaxLevel(int value);

    void check(Connection_ptr connection, 
            TypeDescriptor_ptr message,
            const QList< int >& path);

    void uncheck(Connection_ptr connection, 
            TypeDescriptor_ptr message,
            const QList< int >& path);

signals:

    void checked(Connection_ptr connection, 
            TypeDescriptor_ptr message,
            const QList< int >& path);

    void unchecked(Connection_ptr connection, 
            TypeDescriptor_ptr message,
            const QList< int >& path);

protected:

    QModelIndex toModelIndex(Connection_ptr connection, 
            TypeDescriptor_ptr message,
            const QList< int >& path);

    virtual bool isCheckable(TypeDescriptor_ptr reflective) const;

    Direction m_direction;

    typedef QList< GroupNode_ptr > Nodes_t;
    Nodes_t m_nodes;

    int m_maxLevel;

    bool m_checkable;
};

} // qt
} // gsim

#endif // GSIM_QT_INSTANCEMODEL_HPP

