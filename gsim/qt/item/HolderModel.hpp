// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * HolderModel.hpp
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

#ifndef GSIM_QT_HOLDERMODEL_HPP
#define GSIM_QT_HOLDERMODEL_HPP

#include <QAbstractItemModel>

#include <gsim/qt/export.hpp>
#include <gsim/qt/types.hpp>
#include <gsim/qt/item/ModelNode.hpp>

namespace gsim 
{
namespace qt
{

class GSIM_QT_DECLSPEC HolderModel : 
    public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(bool editable READ isEditable WRITE setEditable)
public:
    HolderModel(TypeDescriptor_ptr descriptor, Holder holder, 
            QObject * parent = 0);
    virtual ~HolderModel();

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

public slots:

    void resetModel();

    bool isEditable() const;
    void setEditable(bool editable);

private slots:

    void resetInternalData();

protected:

    class Data;
    Data * m_data;
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_HOLDERMODEL_HPP */

