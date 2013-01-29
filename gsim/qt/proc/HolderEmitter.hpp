// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * HolderEmitter.hpp
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

#ifndef GSIM_QT_HOLDEREMITTER_HPP
#define GSIM_QT_HOLDEREMITTER_HPP

#include <gsim/qt/export.hpp>
#include <gsim/qt/proc/MessageProcessor.hpp>
#include <QObject>

namespace gsim 
{
namespace qt 
{

class GSIM_QT_DECLSPEC HolderEmitter : public QObject,
    public MessageProcessor
{
    Q_OBJECT
public:

    HolderEmitter(Connection_ptr connection,
            TypeDescriptor_ptr messageDescriptor,
            const QList< int >& path);
    ~HolderEmitter();

    void process(Message_ptr req, 
            Holder hold);
    
signals:

    void append(Message_ptr, 
            Holder);
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_HOLDEREMITTER_HPP */
