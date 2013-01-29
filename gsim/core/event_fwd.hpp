// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * event_fwd.hpp
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

#ifndef GSIM_CORE_EVENT_FWD_HPP
#define GSIM_CORE_EVENT_FWD_HPP

#include <boost/shared_ptr.hpp>

namespace gsim 
{
namespace core 
{

struct event;

typedef boost::shared_ptr< event > event_ptr;

struct message;

typedef boost::shared_ptr< message > message_ptr;

} // namespace core
} // namespace gsim

#endif /* GSIM_CORE_EVENT_FWD_HPP */

