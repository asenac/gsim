// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * event.cpp
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

#ifndef GSIM_CORE_EVENT_HPP
#define GSIM_CORE_EVENT_HPP

#include <string>

#include <gsim/core/event_fwd.hpp>
#include <gsim/core/holder.hpp>
#include <gsim/core/tag.hpp>

namespace gsim 
{
namespace core 
{

struct event
{
};

struct message : public event
{

    message(const std::string& name_, ::gsim::core::holder holder_);

    message(const message& msg);

    const std::string& name() const;
    
    ::gsim::core::holder holder() const;

    message * clone() const;

private:

    const std::string m_name;

    ::gsim::core::holder m_holder;
};

} // namespace core
} // namespace gsim

#endif /* GSIM_CORE_EVENT_HPP */

