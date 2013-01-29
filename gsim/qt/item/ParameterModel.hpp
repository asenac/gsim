#ifndef GSIM_QT_PARAMETERMODEL_HPP
#define GSIM_QT_PARAMETERMODEL_HPP

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

class GSIM_QT_DECLSPEC ParameterModel : 
    public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(bool checkable 
            READ isCheckable
            WRITE setCheckable)
public:

    ParameterModel(QObject *parent = 0);
    virtual ~ParameterModel();

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


    bool isCheckable() const;
    void setCheckable(bool checkable);

    bool isMessageNode(const QModelIndex& index) const;

public slots:

    void initialize(const QString& name, TypeDescriptor_ptr descriptor);

    void check(TypeDescriptor_ptr message,
            const QList< int >& path);

    void uncheck(TypeDescriptor_ptr message,
            const QList< int >& path);

signals:

    void checked(TypeDescriptor_ptr message,
            const QList< int >& path);

    void unchecked(TypeDescriptor_ptr message,
            const QList< int >& path);

protected:

    QModelIndex toModelIndex(TypeDescriptor_ptr message,
            const QList< int >& path);

    virtual bool isCheckable(TypeDescriptor_ptr reflective) const;

    DescriptorNode_ptr m_root;

    bool m_checkable;
};

} // qt
} // gsim

#endif // GSIM_QT_PARAMETERMODEL_HPP

