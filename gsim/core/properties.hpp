// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * properties.hpp
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

#ifndef GSIM_CORE_PROPERTIES_HPP
#define GSIM_CORE_PROPERTIES_HPP

#include <map>
#include <boost/variant.hpp>
#include <gsim/types.hpp>
#include <gsim/core/descriptor_fwd.hpp>

namespace gsim 
{
namespace core 
{

typedef boost::variant< 
        std::string,
        bool,
        float,
        double,
        char,
        unsigned char,
        short,
        unsigned short,
        int32_t,
        uint32_t,
        int64_t,
        uint64_t
    > property_t;

enum ENUM_PROPERTIES
{
    PROP_STRING,
    PROP_BOOL,
    PROP_FLOAT,
    PROP_DOUBLE,
    PROP_CHAR,
    PROP_UCHAR,
    PROP_SHORT,
    PROP_USHORT,
    PROP_INT32,
    PROP_UINT32,
    PROP_INT64,
    PROP_UINT64,
    PROP_MAX
};

struct set_property
{
    set_property(const char *, 
            const char * property,
            const property_t& value);
};

class property_repository
{
public:
    
    ~property_repository();

    static property_repository * instance();

    void set_property(const char * fqn, 
            const char * property,
            const property_t& value);

    template< typename T >
    const T * get_property(const char * fqn, 
            const char * property) const;

    template< typename T >
    const T * get_property(descriptor_ptr descriptor, 
            const char * property) const;

    /**
     * @brief We return a pointer because boost::variant never-empty guarantee
     *        and we need to know whether the property is defined.
     *
     * @param fqn
     * @param property The name of the property.
     *
     * @return A const-pointer to the property value or NULL.
     */
    const property_t * get_property(const char * fqn, 
            const char * property) const;

    const property_t * get_property(descriptor_ptr descriptor, 
            const char * property) const;

protected:

    property_repository();

    typedef std::map< std::string, property_t >
        property_map_t;
    typedef std::map< std::string, property_map_t >
        repository_t;

    repository_t m_repository;
};

struct initializer
{
    typedef void (*initializer_t)();

    initializer(initializer_t init);
};

template< typename T >
const T * property_repository::get_property(
        const char * fqn, const char * property) const
{
    const property_t * p = get_property(fqn, property);

    if (p)
        return boost::get< T >(p);

    return NULL;
}

template< typename T >
const T * property_repository::get_property(
        descriptor_ptr descriptor, const char * property) const
{
    const property_t * p = get_property(descriptor, property);

    if (p)
        return boost::get< T >(p);

    return NULL;
}

} // namespace core
} // namespace gsim

#define GSIM_STR(tok) #tok
#define GSIM_PROPERTY(fqn, property, value)                \
    { ::gsim::core::set_property __tmp(                    \
            GSIM_STR(fqn),   				               \
            property, value); }                            \
    /***/

#define GSIM_INITIALIZER                                        \
    void __initializer();                                       \
    ::gsim::core::initializer _initializer(&__initializer);     \
    void __initializer                                          \
    /***/

#endif /* GSIM_CORE_PROPERTIES_HPP */

