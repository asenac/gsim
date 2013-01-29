// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * MessageProcessor.hpp
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

#ifndef GSIM_QT_MESSAGEPROCESSOR_HPP
#define GSIM_QT_MESSAGEPROCESSOR_HPP

#include <gsim/qt/types.hpp>
#include <gsim/qt/export.hpp>

namespace gsim 
{
namespace qt 
{

class GSIM_QT_DECLSPEC MessageProcessor
{
public:

    virtual ~MessageProcessor();

    virtual void process(Message_ptr req, Holder value) = 0;

    Connection_ptr connection() const;

    const ReflectivePath_t& path() const;

    TypeDescriptor_ptr messageDescriptor() const;
    
    TypeDescriptor_ptr descriptor() const;

protected:

    MessageProcessor(Connection_ptr connection,
            TypeDescriptor_ptr messageDescriptor,
            const ReflectivePath_t& path);

    Connection_ptr m_connection;
    const ReflectivePath_t m_path;
    TypeDescriptor_ptr m_messageDescriptor;
    TypeDescriptor_ptr m_descriptor;
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_MESSAGEPROCESSOR_HPP */

