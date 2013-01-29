// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * AbstractInputTool.hpp
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

#ifndef GSIM_QT_ABSTRACTINPUTTOOL_HPP
#define GSIM_QT_ABSTRACTINPUTTOOL_HPP

#include <QtGui>
#include <gsim/qt/types.hpp>
#include <gsim/qt/item/MessageModel.hpp>
#include <gsim/qt/proc/MessageProcessor.hpp>
#include <gsim/qt/tools/AbstractTool.hpp>
#include <map>

#include <gsim/qt/export.hpp>

namespace gsim 
{
namespace qt 
{
class SortableGroup;
class SortableGroupItem;

class GSIM_QT_DECLSPEC AbstractInputItem : public QWidget
{
    Q_OBJECT
protected:

    AbstractInputItem(Connection_ptr connection_,
            TypeDescriptor_ptr message_,
            const QList< int >& path_, 
            QWidget * parent = 0);

public:

    virtual ~AbstractInputItem();

    Connection_ptr connection() const;

    TypeDescriptor_ptr message() const;

    const QList< int >& path() const;

    MessageProcessor_ptr processor() const;

    void save(QVariant& settings);
    void load(const QVariant& settings);

    virtual void start();
    virtual void reset();

signals:

    void addProcessor(MessageProcessor_ptr);
    void removeProcessor(MessageProcessor_ptr);

protected:

    MessageProcessor_ptr m_processor;
    Connection_ptr m_connection;
    TypeDescriptor_ptr m_message;
    const QList< int > m_path;
};

class GSIM_QT_DECLSPEC AbstractInputTool : public AbstractTool
{
    Q_OBJECT
public:

    AbstractInputTool(QWidget * parent = 0);
    virtual ~AbstractInputTool();

    void save(QVariant& settings);
    void load(const QVariant& settings);

    void setModel(MessageModel * model);

protected:
    
    virtual AbstractInputItem * createItem(
            Connection_ptr connection, 
            TypeDescriptor_ptr message,
            const QList< int >& path) = 0;

    virtual MessageModel * createModel();

public slots:

    void clear();

    void registerInstance(Connection_ptr connection);

    void unregisterInstance(Connection_ptr connection);

    AbstractInputItem * createAbstractInputItem(
            Connection_ptr connection, 
            TypeDescriptor_ptr message,
            const QList< int >& path);

    void deleteAbstractInputItem(
            Connection_ptr connection, 
            TypeDescriptor_ptr message,
            const QList< int >& path);

protected slots:

    void deleteRequested(gsim::qt::SortableGroupItem *);

protected:

    MessageModel * m_model;
    qt::SortableGroup * m_group;

    typedef std::pair< Connection *, TypeDescriptor_ptr > key_t;
    typedef std::map< key_t, QList< AbstractInputItem * > > map_t;
    typedef std::map< AbstractInputItem *, key_t > inverse_map_t;

    map_t m_map;
    inverse_map_t m_inverse_map;

    QTreeView * m_view;
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_ABSTRACTINPUTTOOL_HPP */
