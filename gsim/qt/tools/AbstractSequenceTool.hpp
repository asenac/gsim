// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * AbstractSequence.hpp
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

#ifndef GSIM_QT_ABSTACTSEQUENCETOOL_HPP
#define GSIM_QT_ABSTACTSEQUENCETOOL_HPP

#include <QtGui>
#include <gsim/qt/types.hpp>
#include <gsim/qt/export.hpp>
#include <gsim/qt/widgets/CustomLayouts.hpp>
#include <gsim/qt/widgets/SortableGroup.hpp>
#include <gsim/qt/item/MessageModel.hpp>
#include <gsim/qt/tools/AbstractTool.hpp>

namespace gsim 
{
namespace qt 
{

class OperationsView;

class GSIM_QT_DECLSPEC AbstractSequenceItem : 
    public qt::AbstractItemFrame
{
    Q_OBJECT
protected:

    AbstractSequenceItem(Connection_ptr connection,
            TypeDescriptor_ptr message,
            QWidget * parent = 0);

public:

    virtual ~AbstractSequenceItem();

    Connection_ptr connection() const;
    TypeDescriptor_ptr message() const;

    void save(QVariant& settings);
    void load(const QVariant& settings);

public slots:

    virtual void showDetails(bool show) = 0;

signals:

    void doDelete();
    void up();
    void down();

protected:

    virtual void doSave(QVariantMap& settings) = 0;
    virtual void doLoad(const QVariantMap& settings) = 0;

    Connection_ptr m_connection;
    TypeDescriptor_ptr m_message;
    QLineEdit * m_title;
};

class GSIM_QT_DECLSPEC AbstractSequence : public QWidget
{
    Q_OBJECT
public:
    AbstractSequence(const QString& name, QWidget * parent = 0);
    virtual ~AbstractSequence();

    const QString& getName() const;
    void setName(const QString& name);

    void save(QVariant& settings);
    void load(const QVariant& settings);

    /**
     * @brief Remove all the items related to an instance.
     *
     * @param id The identifier of the instance.
     */
    void removeInstance(Connection_ptr id);

public slots:

    void appendItem(AbstractSequenceItem * item);
    void forgetItem(AbstractSequenceItem * item);

    void deleteItem(AbstractSequenceItem * item);
    void moveUpItem(AbstractSequenceItem * item);
    void moveDownItem(AbstractSequenceItem * item);

    void stop();

private slots:

    void startOrStopAll(bool checked);

    void deleteItem();
    void moveUpItem();
    void moveDownItem();

signals:

    void modified();

protected:

    /**
     * @brief The sequence name. 
     */
    QString m_name;

    qt::CustomVLayout * m_layout;

    typedef QList< AbstractSequenceItem * > items_t;
    items_t m_items;

    QScrollArea * m_scroll;

    QPushButton * m_stBtn;
};

/**
 * @brief Abstraction for message sequences tool.
 *
 * It is composed of a QTabWidget with a tab per sequence.
 * Every sequence consists in an ordered set of AbstratSequenceItem
 * instances.
 */
class GSIM_QT_DECLSPEC AbstractSequenceTool : 
    public AbstractTool
{
    Q_OBJECT
public:
    AbstractSequenceTool(QWidget * parent = 0);
    virtual ~AbstractSequenceTool();

    void save(QVariant& settings);
    void load(const QVariant& settings);

public slots:

    void registerInstance(Connection_ptr connection);
    void unregisterInstance(Connection_ptr id);

    /**
     * @brief Append a new item to the current sequence. It uses
     * createAbstractItem to create the item.
     *
     * If no current creates a new sequence.
     *
     * @param connection
     * @param op
     *
     * @return The new item or NULL if invalid connection or message.
     */
    AbstractSequenceItem * appendAbstractItem(
            Connection_ptr connection,
            TypeDescriptor_ptr op);

    AbstractSequence* createSequence();

    void closeSequence(int idx);

    void showContextMenu(const QPoint& pos);

    void saveCurrentSequence();
    void loadSequence();

    // Current item slots
    void saveCurrentItem();
    void loadCurrentItem();

    void duplicateCurrentItem();
    void deleteCurrentItem();

    void moveCurrentItemTo(int idx);
    // End current item slots

    void setTreeVisible(bool visible);

    /**
     * @brief Displays a dialog for setting the current sequence name.
     */
    void showSetName();

    /**
     * @brief Stop all the sequences.
     */
    void stop();

private slots:

    void sequenceModified();

protected:

    /**
     * @brief Must create an instance of a subclass of AbstractSequenceItem.
     *
     * @param connection The associated instance.
     * @param op The associated message.
     *
     * @return The created instance or NULL if invalid connection or message.
     */
    virtual AbstractSequenceItem * createAbstractItem(
            Connection_ptr connection, 
            TypeDescriptor_ptr op) = 0;

    void regenerateMoveTo(int idx);

    MessageModel m_model;

    OperationsView * m_view;
    QTabWidget * m_tabs;

    typedef QList< AbstractSequence * > sequences_t;
    sequences_t m_sequences;

    QMenu * m_menu;
    QMenu * m_menuCurrentItem;
    QMenu * m_menuCurrentItemMoveTo;
    QSignalMapper * m_moveToSignalMapper;

    QString m_filter;
    QString m_extension;

    /**
     * @brief Current item.
     */
    AbstractSequenceItem * m_currentItem;

    /**
     * @brief Only enabled when a current item.
     */
    QList< QAction * > m_currentItemActions;
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_ABSTACTSEQUENCETOOL_HPP */

