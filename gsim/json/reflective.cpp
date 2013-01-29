// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * reflective.cpp
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

#include "reflective.hpp"
#include <gsim/json/parser.hpp>
#include <gsim/json/writer.hpp>

#include <gsim/core/holder.hpp>
#include <gsim/core/descriptor.hpp>
#include <gsim/core/descriptor_utils.hpp>

#include <iomanip>

using namespace gsim::json;
using namespace gsim::core;

reflective_helper::reflective_helper(
		gsim::core::descriptor_ptr reflective,
        gsim::core::holder holder) :
    m_reflective(reflective), m_holder(holder), m_currentIndex(0)
{
}

reflective_helper::~reflective_helper()
{
}

void reflective_helper::new_double(double value_)
{
    const descriptor_type type = m_reflective->get_type();

    switch(type)
    {
        case TYPE_DOUBLE:
            m_holder.to_value< double >() = (double) value_;
            break;
        case TYPE_FLOAT:
            m_holder.to_value< float >() = (float) value_;
            break;
        case TYPE_OCTET:
            m_holder.to_value< unsigned char >() = (unsigned char) value_;
            break;
        case TYPE_CHAR:
            m_holder.to_value< char >() = (char) value_;
            break;
        case TYPE_SHORT:
            m_holder.to_value< short >() = (short) value_;
            break;
        case TYPE_USHORT:
            m_holder.to_value< unsigned short >() = 
                (unsigned short) value_;
            break;
        case TYPE_LONG:
            m_holder.to_value< int32_t >() = (int32_t) value_;
            break;
        case TYPE_ULONG:
            m_holder.to_value< uint32_t >() = (uint32_t) value_;
            break;
        case TYPE_LONGLONG:
            m_holder.to_value< int64_t >() = (int64_t) value_;
            break;
        case TYPE_ULONGLONG:
            m_holder.to_value< uint64_t >() = (uint64_t) value_;
            break;
        default:
            throw "Error!";
    }
}

void reflective_helper::new_string(const std::string& d)
{
    const descriptor_type type = m_reflective->get_type();

    switch(type)
    {
        case TYPE_STRING:
            m_reflective->from_string(m_holder, d);
            break;

        case TYPE_ENUM:
            {
                // Esta es la forma de obtener el valor de un enumerado
                // a partir del string de su literal
                const unsigned int count = 
                    m_reflective->get_children_count();
                unsigned int idx = m_reflective->get_child_index(d.c_str());

                if (idx < count)
                {
                    core::holder tmp(
                            m_reflective->get_child_value(m_holder, idx));

                    m_reflective->copy(tmp, m_holder);
                }
            }
            break;

        case TYPE_ARRAY:

            if (m_reflective->get_slice()->get_type() == TYPE_CHAR)
            {
                m_reflective->from_string(m_holder, d);
                break;
            }

        default:
            throw "Error!";
    }
}

void reflective_helper::new_bool(bool d)
{
    const descriptor_type type = m_reflective->get_type();

    switch(type)
    {
        case TYPE_BOOL:
            m_holder.to_value< bool >() = d;
            break;
        default:
            throw "Error!";
    }
}

void reflective_helper::new_null()
{
    const descriptor_type type = m_reflective->get_type();

    switch(type)
    {
        default:
            throw "Error!";
    }
}

// For structs
helper::helper_base* reflective_helper::new_child(const std::string& name)
{
    if (m_reflective->get_type() == TYPE_STRUCT)
    {
        const unsigned int count = m_reflective->get_children_count();
        for (unsigned int i = 0; i < count; i++) 
            if (name == m_reflective->get_child_name(i))
            {
                return new reflective_helper(m_reflective->get_child(i),
                        m_reflective->get_child_value(m_holder, i));
            }
    }

    throw "Error!";
}

// For arrays
helper::helper_base* reflective_helper::new_child()
{
    if (m_reflective->is_repeated())
    {
        // Increments its length
        if (m_reflective->is_variable_length())
        {
            m_reflective->set_length(m_holder, m_currentIndex + 1);
        }
        else
        {
            unsigned int length = m_reflective->get_length(m_holder);

            if (m_currentIndex >= length)
                throw "Error!";
        }

        return new reflective_helper(m_reflective->get_slice(),
                m_reflective->get_child_value(m_holder, m_currentIndex++));
    }

    throw "Error!";
}

// Parse
bool gsim::json::parse(core::descriptor_ptr reflective, 
        core::holder& holder, const char * str, size_t size)
{
    helper::helper_base * initial_helper = 
        new reflective_helper(reflective, holder);

    semantic_state _ss(initial_helper);
    state _st(_ss, str, size);

    return gsim::json::parser::grammar::gram::match(_st);
}

bool gsim::json::parse(core::descriptor_ptr reflective, 
        core::holder& holder, std::istream& in)
{
    helper::helper_base * initial_helper = 
        new reflective_helper(reflective, holder);

    semantic_state _ss(initial_helper);
    istream_state _st(_ss, in);

    return gsim::json::parser::grammar::gram::match(_st);
}


void gsim::json::write(std_writer_t& w, 
        gsim::core::descriptor_ptr reflective, 
        gsim::core::holder holder)
{
    using namespace gsim::core;

    const descriptor_type type = reflective->get_type();

    switch(type)
    {
        case TYPE_ENUM:
            {
                const char * literal = 
                    core::utils::get_enum_literal(holder);

                if (literal)
                {
                    // Obtiene su nombre a partir de su indice
                    w.new_string(literal);
                }
                else
                {
                    w.new_string("Invalid value!");
                }
            }
            break;

        case TYPE_BOOL:
            w.new_bool(holder.to_value< bool >());
            break;

        case TYPE_DOUBLE:
            w.new_double((double) holder.to_value< double >());
            break;
        case TYPE_FLOAT:
            w.new_double((double) holder.to_value< float >());
            break;
        case TYPE_OCTET:
            w.new_double((double) holder.to_value< unsigned char >());
            break;
        case TYPE_CHAR:
            w.new_double((double) holder.to_value< char >());
            break;
        case TYPE_SHORT:
            w.new_double((double) holder.to_value< short >());
            break;
        case TYPE_USHORT:
            w.new_double((double) holder.to_value< unsigned short >());
            break;
        case TYPE_LONG:
            w.new_double((double) holder.to_value< int32_t >());
            break;
        case TYPE_ULONG:
            w.new_double((double) holder.to_value< uint32_t >());
            break;
        case TYPE_LONGLONG:
            w.new_double((double) holder.to_value< int64_t >());
            break;
        case TYPE_ULONGLONG:
            w.new_double((double) holder.to_value< uint64_t >());
            break;

        case TYPE_ARRAY:

            if (reflective->get_slice()->get_type() == TYPE_CHAR)
            {
                w.new_string(reflective->to_string(holder).c_str());
                break;
            }

        case TYPE_SEQUENCE:
            {
                const unsigned int count = 
                    reflective->get_length(holder);

                w.array_start();

                for (unsigned int i = 0; i < count; i++) 
                {
                    // Value
                    write(w, reflective->get_slice(),
                            reflective->get_child_value(holder, i));
                }

                w.array_end();
            }
            break;

        case TYPE_STRUCT:
            {
                const unsigned int count = 
                    reflective->get_children_count();

                w.object_start();

                for (unsigned int i = 0; i < count; i++) 
                {
                    // Name
                    w.new_string(reflective->get_child_name(i));

                    // Value
                    write(w, reflective->get_child(i),
                            reflective->get_child_value(holder, i));
                }

                w.object_end();
            }
            break;

        case TYPE_STRING:
            {
                std::string str = reflective->to_string(holder);
                w.new_string(str.c_str());
            }
            break;
        default:
            break;
    }
}

// Write
void gsim::json::write(std::ostream& os, 
        core::descriptor_ptr reflective, 
        core::holder& holder, bool indent)
{
    os << std::setprecision(100);

    std_writer_t w(os, indent);

    write(w, reflective, holder);
}


