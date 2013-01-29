// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * DumpTool.hpp
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

#ifndef GSIM_QT_DUMPTOOL_HPP
#define GSIM_QT_DUMPTOOL_HPP

#include <QtGui>
#include <gsim/qt/export.hpp>
#include <gsim/qt/tools/AbstractInputTool.hpp>
#include <gsim/qt/proc/MessageProcessor.hpp>

namespace gsim 
{
namespace qt 
{

class GSIM_QT_DECLSPEC Dumper : public AbstractInputItem
{
    Q_OBJECT
public:

    Dumper(Connection_ptr connection_,
            TypeDescriptor_ptr message_,
            const QList< int >& path_, 
            QWidget * parent = 0);
    virtual ~Dumper();

    void save(QVariant& settings);
    void load(const QVariant& settings);

public slots:

    void doStart(bool);

protected slots:

    void browse();
    void setEnabled(bool);

protected:

    class Data;
    Data * m_data;
};

class GSIM_QT_DECLSPEC DumpTool : public AbstractInputTool
{
    Q_OBJECT
public:

    DumpTool(QWidget * parent = 0);
    virtual ~DumpTool();

protected:

    AbstractInputItem * createItem(
            Connection_ptr connection, 
            TypeDescriptor_ptr message,
            const QList< int >& path);

};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_DUMPTOOL_HPP */
