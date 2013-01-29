// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * OperationsView.cpp
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

#include "OperationsView.hpp"
#include <gsim/qt/item/MessageModel.hpp>
#include <gsim/qt/model/Connection.hpp>

using namespace gsim::qt;

// View
OperationsView::OperationsView(QWidget * parent) :
    QTreeView(parent)
{
    connect(this, SIGNAL(doubleClicked(const QModelIndex&)),
        this, SLOT(clicked(const QModelIndex&)));
}

OperationsView::~OperationsView()
{
}

void OperationsView::clicked(const QModelIndex& index)
{
    MessageModel * src = dynamic_cast< MessageModel * >(model());

    if (src)
    {
        Connection_ptr instance = src->getInstance(index);

        if (src->isGroupNode(index))
        {
            emit selectedInstance(instance);
        }
        else if (src->isMessageNode(index))
        {
            QList< int > list;
            list << index.row();

            TypeDescriptor_ptr op = 
                instance->descriptor()->out()->get_descriptor_by_index(index.row());

            emit selectedItem(instance, op, list);

            emit selectedOperation(instance, op);
        }
        else
        {
            QList< int > list;

            QModelIndex current = index;
            while(current.parent().isValid())
            {
                list.push_front(current.row());
                current = current.parent();
            }

            TypeDescriptor_ptr op = 
                instance->descriptor()->out()->get_descriptor_by_index(list.front());

            emit selectedItem(instance, op, list);
        }
    }
}

