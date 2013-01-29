// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * SenderSequence.hpp
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

#ifndef GSIM_QT_SENDERSEQUENCE_HPP
#define GSIM_QT_SENDERSEQUENCE_HPP

#include <gsim/qt/export.hpp>
#include <gsim/qt/tools/AbstractSequenceTool.hpp>

namespace gsim 
{
namespace qt 
{

class MessageSender;

class GSIM_QT_DECLSPEC SenderSequenceItem : 
    public AbstractSequenceItem
{
    Q_OBJECT
public:
    SenderSequenceItem(Connection_ptr connection,
            TypeDescriptor_ptr operation,
            QWidget * parent = 0);
    virtual ~SenderSequenceItem();

    MessageSender * sender() const;

public slots:
    
    void showDetails(bool show);

protected:

    void doSave(QVariantMap& map);
    void doLoad(const QVariantMap& map);

    MessageSender * m_dlg;
};

class GSIM_QT_DECLSPEC SenderSequenceTool : 
    public AbstractSequenceTool
{
    Q_OBJECT
public:
    SenderSequenceTool(QWidget * parent = 0);
    virtual ~SenderSequenceTool();

public slots:

    void saveCurrentItemFormValue();
    void loadCurrentItemFormValue();

protected:

    AbstractSequenceItem * createAbstractItem(
            Connection_ptr connection, 
            TypeDescriptor_ptr op);
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_SENDERSEQUENCE_HPP */

