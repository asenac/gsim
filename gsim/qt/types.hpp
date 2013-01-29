// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * types.hpp
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

#ifndef GSIM_QT_TYPES_HPP
#define GSIM_QT_TYPES_HPP

#include <QList>
#include <QString>

#include <gsim/core/event_fwd.hpp>
#include <gsim/core/descriptor.hpp>

namespace gsim 
{
namespace qt 
{

typedef core::descriptor_ptr TypeDescriptor_ptr;
typedef core::holder Holder;

class Connection;
typedef boost::shared_ptr< Connection > Connection_ptr;
typedef ::gsim::core::message_ptr Message_ptr; 
typedef ::gsim::core::connection_descriptor_ptr ConnectionDescriptor_ptr; 

typedef QList< int > ReflectivePath_t;

class MessageProcessor;
typedef boost::shared_ptr< MessageProcessor > MessageProcessor_ptr;

QString getMessageName(TypeDescriptor_ptr);

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_TYPES_HPP */

