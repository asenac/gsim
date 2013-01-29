// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * ReflectiveItemDelegate.cpp
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

#include "ReflectiveItemDelegate.hpp"
#include <gsim/qt/reflective/ReflectiveWidget.hpp>
#include <gsim/qt/reflective/WidgetFactory.hpp>
#include <gsim/qt/item/ModelNode.hpp>

using namespace gsim::qt;

namespace  
{

    Node * getNode(const QModelIndex& index)
    {
        int col = index.column();

        AbstractNode * aNode = 
            static_cast< AbstractNode * >(index.internalPointer());

        MetaNode * mNode = dynamic_cast< MetaNode * >(aNode);

        if (mNode)
        {
            if (col > 0 && col <= (int) mNode->brothers.size())
                return mNode->brothers[col - 1].get();
            else
                return NULL;
        }

        return dynamic_cast< Node * >(aNode);
    }

} // namespace 

ReflectiveItemDelegate::ReflectiveItemDelegate(QObject * parent) :
    QStyledItemDelegate(parent)
{
}

ReflectiveItemDelegate::~ReflectiveItemDelegate()
{
}

QWidget * ReflectiveItemDelegate::createEditor(QWidget *parent, 
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
    Node * node = getNode(index);

    if (node)
    {
        QWidget * w = WidgetFactoryRepository::create(node->descriptor, parent);
        ReflectiveWidget * r = dynamic_cast< ReflectiveWidget * >(w);

        // Fallo de diseño
        if (r && r->isBigWidget())
        {
            delete w;
            return NULL;
        }

        return w;
    }

    return NULL;
}

void ReflectiveItemDelegate::setEditorData(QWidget *editor, 
        const QModelIndex &index) const
{
    Node * node = getNode(index);
    ReflectiveWidget * w = dynamic_cast< ReflectiveWidget * >(editor);

    if (node && w)
    {
        w->fromHolder(node->holder);
    }
}

void ReflectiveItemDelegate::setModelData(QWidget *editor, 
        QAbstractItemModel *model,
        const QModelIndex &index) const
{
    Node * node = getNode(index);
    ReflectiveWidget * w = dynamic_cast< ReflectiveWidget * >(editor);

    if (node && w)
    {
        w->toHolder(node->holder);
    }
}

void ReflectiveItemDelegate::updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, 
        const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
    editor->setFont(option.font);
}

void ReflectiveItemDelegate::paint(QPainter * painter, 
        const QStyleOptionViewItem& option, 
        const QModelIndex& index ) const
{
    QStyledItemDelegate::paint(painter, option, index);

#if 0
    Node * node = getNode(index);

    if (node) 
    {
    }
    else
    {
        QStyledItemDelegate::paint(painter, option, index);
    }
#endif
}

