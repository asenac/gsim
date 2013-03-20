// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * parser.hpp
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

#ifndef GSIM_JSON_PARSER_HPP
#define GSIM_JSON_PARSER_HPP

#include <list>
#include <boost/shared_ptr.hpp>
#include <gsim/json/helper.hpp>
#include <gsim/json/detail/json.hpp>
#include <gsim/json/detail/json_parse.hpp>
#include <gsim/json/detail/json_writer.hpp>

#include <string>

namespace gsim
{
namespace json
{

struct semantic_state
{
    typedef gsim::json::match_pair match_pair;
    typedef boost::shared_ptr< helper::helper_base > helper_ptr;
    std::list< helper_ptr > m_helpers_stack;

    struct state_struct
    {
        gsim::json::writer::State s;
        bool first_in_object_or_array;
    };
    std::list<state_struct> state_stack;

    semantic_state(helper::helper_base * initial_helper)
    {
        m_helpers_stack.push_back(helper_ptr(initial_helper));

        state_struct ss;
        ss.s = gsim::json::writer::BEGIN;
        ss.first_in_object_or_array = false;
        state_stack.push_back(ss);
    }

    inline void new_null()
    {
        _check_pre();

        m_helpers_stack.back()->new_null();
        _consume();

        _check_post();
    }

    inline void new_double(double d)
    {
        _check_pre();

        m_helpers_stack.back()->new_double(d);
        _consume();

        _check_post();
    }

    inline void new_string(std::string  const& p)
    {
        namespace w = gsim::json::writer;

        std::string val(p);
        gsim::json::parser::decode (val);

        _check_pre();

        if (state_stack.back().s != w::IN_OBJECT_AWAIT_ID)
        {
            m_helpers_stack.back()->new_string(val);
            _consume();
        }
        else
        {
            helper_ptr parent = m_helpers_stack.back();
            m_helpers_stack.push_back(helper_ptr(parent->new_child(val)));
        }

        _check_post();
    }

    inline void new_bool(bool b)
    {
        _check_pre();

        m_helpers_stack.back()->new_bool(b);
        _consume();

        _check_post();
    }

    inline void new_blob(match_pair const& p)
    {
        _check_pre();

        _check_post();
    }

    inline void object_start()
    {
        namespace w = gsim::json::writer;

        _check_pre();

        state_struct ss;
        ss.s = w::IN_OBJECT_AWAIT_ID;
        ss.first_in_object_or_array = true;
        state_stack.push_back(ss);
    }

    inline void object_end()
    {
        state_stack.pop_back();
        _consume();
        _check_post();
    }

    inline void array_start()
    {
        namespace w = gsim::json::writer;

        _check_pre();

        state_struct ss;
        ss.s = w::IN_ARRAY_AWAIT_VALUE;
        ss.first_in_object_or_array = true;
        state_stack.push_back(ss);
    }

    inline void array_end()
    {
        state_stack.pop_back();
        _consume();
        _check_post();
    }

    inline void _check_pre()
    {
        namespace w = gsim::json::writer;

        state_struct& ss = state_stack.back();
        if (ss.s == w::IN_OBJECT_AWAIT_ID ||
                ss.s == w::IN_ARRAY_AWAIT_VALUE)
        {
            if (!ss.first_in_object_or_array)
                ;
            else
                ss.first_in_object_or_array = false;

            if (ss.s == w::IN_ARRAY_AWAIT_VALUE)
            {
                helper_ptr parent = m_helpers_stack.back();
                m_helpers_stack.push_back(helper_ptr(parent->new_child()));
            }
        }
        else if (ss.s == w::IN_OBJECT_AWAIT_VALUE)
        {
        }
    }

    inline void _check_post()
    {
        namespace w = gsim::json::writer;

        state_struct& ss = state_stack.back();
        if (ss.s == w::IN_OBJECT_AWAIT_VALUE)
            ss.s = w::IN_OBJECT_AWAIT_ID;
        else if (ss.s == w::IN_OBJECT_AWAIT_ID)
            ss.s = w::IN_OBJECT_AWAIT_VALUE;
    }

    inline void _consume()
    {
        m_helpers_stack.pop_back();
    }
};

typedef gsim::json::parser::State< semantic_state > state;
typedef gsim::json::parser::IStreamState< semantic_state > istream_state;

inline bool parse(helper::helper_base * initial_helper, 
        const char * str, size_t size)
{
    semantic_state _ss(initial_helper);
    state _st(_ss, str, size);

    return gsim::json::parser::grammar::gram::match(_st);
}

} // namespace json
} // namespace gsim

#endif /* GSIM_JSON_PARSER_HPP */

