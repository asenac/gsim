// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * adapted.hpp
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

#ifndef GSIM_ADAPTED_HPP
#define GSIM_ADAPTED_HPP

// boost::fusion::result_of::value_at
#include <boost/fusion/sequence/intrinsic/value_at.hpp>
#include <boost/fusion/include/value_at.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/seq/size.hpp>

#include <gsim/types.hpp>
#include <gsim/core/mpl.hpp>
#include <gsim/core/descriptor_fwd.hpp>

#define GSIM_ADAPT_FILLER_0(X, Y)                              \
    ((X, Y)) GSIM_ADAPT_FILLER_1
#define GSIM_ADAPT_FILLER_1(X, Y)                              \
    ((X, Y)) GSIM_ADAPT_FILLER_0
#define GSIM_ADAPT_FILLER_0_END
#define GSIM_ADAPT_FILLER_1_END

#define GSIM_ADAPT_ENUM_MEMBER(r, data, elem)                       \
    { BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(2, 0, elem)), BOOST_PP_TUPLE_ELEM(2, 1, elem)}, \
    /***/

#define GSIM_ADAPT_ENUM(TYPE, LITERALS)                             \
    namespace gsim { namespace adapted {                            \
        template< > struct enumeration< TYPE  > {                   \
            static const unsigned int size =                        \
                BOOST_PP_SEQ_SIZE(BOOST_PP_CAT(GSIM_ADAPT_FILLER_0 LITERALS,_END));\
            struct pair {                                           \
                const char * name;                                  \
                TYPE value;                                         \
            };                                                      \
            static const pair* value() {                            \
                static const pair _pairs[] = {                      \
                    BOOST_PP_SEQ_FOR_EACH(GSIM_ADAPT_ENUM_MEMBER, _, BOOST_PP_CAT(GSIM_ADAPT_FILLER_0 LITERALS,_END)) \
                    {NULL, static_cast< TYPE >(0)}                  \
                };                                                  \
                return _pairs;                                      \
            }                                                       \
        };                                                          \
    } }                                                             \
    /***/

#define GSIM_ADAPT_MEMBER_IMPL(r, TYPE, i, elem)                    \
    template < > struct struct_member< TYPE, i > {                  \
        static inline const char * fqn() {                          \
            return BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(2, 0, elem)); \
        }                                                           \
    };                                                              \
    /***/

#define GSIM_ADAPT_STRUCT_IMPL(TYPE, MEMBERS)                       \
    BOOST_PP_SEQ_FOR_EACH_I(GSIM_ADAPT_MEMBER_IMPL, TYPE, MEMBERS)    \
    /***/

#define GSIM_ADAPT_STRUCT(TYPE, MEMBERS)                            \
    BOOST_FUSION_ADAPT_STRUCT(TYPE, MEMBERS)                        \
    namespace gsim { namespace adapted {                            \
    template< > struct struct_helper< TYPE > {                      \
        static inline const char * fqn() {                          \
            return BOOST_PP_STRINGIZE(TYPE);                        \
        }                                                           \
    };                                                              \
    GSIM_ADAPT_STRUCT_IMPL(TYPE, BOOST_PP_CAT(GSIM_ADAPT_FILLER_0 MEMBERS,_END)) \
    }}                                                              \
    /***/

#define GSIM_DEFINE_MEMBER_IMPL(r, TYPE, i, elem)                    \
    BOOST_PP_TUPLE_ELEM(2, 0, elem) BOOST_PP_TUPLE_ELEM(2, 1, elem); \
    /***/

#define GSIM_DEFINE_STRUCT_IMPL(MEMBERS)                       \
    BOOST_PP_SEQ_FOR_EACH_I(GSIM_DEFINE_MEMBER_IMPL, TYPE, MEMBERS)  \
    /***/

#define GSIM_DEFINE_INSTANCE(TYPE, MEMBERS)                         \
    namespace _gsim_ { struct TYPE {                                \
        GSIM_DEFINE_STRUCT_IMPL(BOOST_PP_CAT(GSIM_ADAPT_FILLER_0 MEMBERS,_END)) \
    }; }                                                            \
    namespace gsim { namespace adapted {                            \
    template< > struct struct_helper< _gsim_::TYPE > {              \
        static inline const char * fqn() {                          \
            return BOOST_PP_STRINGIZE(_gsim_::TYPE);                \
        }                                                           \
    };                                                              \
    GSIM_ADAPT_STRUCT_IMPL(_gsim_::TYPE, BOOST_PP_CAT(GSIM_ADAPT_FILLER_0 MEMBERS,_END)) \
    }}                                                              \
    /***/

namespace gsim
{
namespace adapted
{

struct empty_serialization
{
    template< typename Archive >
    void serialize(Archive&, const unsigned int)
    {
    }
};

template < typename T >
struct enumeration;

template< typename Struct >
struct struct_helper;

template< typename Struct, unsigned int member >
struct struct_member;

} // adapted
} // gsim

#endif // GSIM_ADAPTED_HPP

