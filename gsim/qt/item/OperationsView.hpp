// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * OperationsView.hpp
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

#ifndef GSIM_QT_OPERATIONSVIEW_HPP
#define GSIM_QT_OPERATIONSVIEW_HPP

#include <QTreeView>
#include <gsim/qt/types.hpp>
#include <gsim/qt/export.hpp>

namespace gsim 
{
namespace qt 
{

class GSIM_QT_DECLSPEC OperationsView : public QTreeView
{
    Q_OBJECT
public:

    OperationsView(QWidget * parent = 0);
    virtual ~OperationsView();

protected slots:

    void clicked(const QModelIndex&);

signals:

    void selectedOperation(Connection_ptr, 
            TypeDescriptor_ptr);

    void selectedItem(Connection_ptr, 
            TypeDescriptor_ptr,
            const QList< int >&);

    void selectedInstance(Connection_ptr);

};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_OPERATIONSVIEW_HPP */

