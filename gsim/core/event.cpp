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

#include "event.hpp"

using namespace gsim::core;

message::message(const std::string& name_, ::gsim::core::holder holder_) :
    m_name(name_),
    m_holder(holder_)
{
}

message::message(const message& msg) :
    m_name(msg.m_name),
    m_holder(msg.m_holder)
{
}

const std::string& message::name() const
{
    return m_name;
}

::gsim::core::holder message::holder() const
{
    return m_holder;
}

message * message::clone() const
{
    return new message(m_name, m_holder.clone());
}
