// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * descriptor.hpp
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

#ifndef GSIM_CORE_DESCRIPTOR_HPP
#define GSIM_CORE_DESCRIPTOR_HPP

#include <map>

#include <gsim/types.hpp>
#include <gsim/core/tag.hpp>
#include <gsim/core/holder.hpp>

namespace gsim 
{
namespace core 
{

enum descriptor_type
{
    TYPE_INVALID,
    TYPE_CHAR,
    TYPE_OCTET,
    TYPE_BOOL,
    TYPE_ENUM,
    TYPE_SHORT,
    TYPE_USHORT,
    TYPE_LONG,
    TYPE_ULONG,
    TYPE_LONGLONG,
    TYPE_ULONGLONG,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_STRING,
    TYPE_WSTRING,
    TYPE_STRUCT,
    TYPE_ARRAY,
    TYPE_UNION,
    TYPE_SEQUENCE
};


/**
 * @brief Describes a type or a member.
 */
class descriptor_base
{
public:

    virtual ~descriptor_base();

    /**
     * @brief Get the type descriptor.
     *
     * @return A special instance of the same class but without parent.
     */
    virtual descriptor_ptr get_type_descriptor() const = 0;

    descriptor_ptr get_parent() const;
    unsigned int get_child_index() const;

    virtual unsigned int get_children_count() const;
    virtual const char * get_child_name(unsigned int idx) const;
    virtual descriptor_ptr get_child(unsigned int idx) const;
    virtual descriptor_ptr get_child_by_name(const char *) const;
    virtual unsigned int get_child_index(const char *) const;

    bool is_fixed_size() const;
    virtual bool is_repeated() const;
    virtual bool is_variable_length() const;
    virtual bool is_primitive() const;
    virtual bool is_enum() const;

    virtual descriptor_type get_type() const;

    // Requires is_repeated
    virtual descriptor_ptr get_slice() const;

    virtual holder create_holder() const;

    // dynamic information
    virtual unsigned int get_length(holder const& value) const;
    virtual void set_length(holder& value, unsigned int length) const;

    virtual holder get_child_value(holder& value, 
        unsigned int idx) const;

    virtual void set_child_value(holder& value, 
        unsigned int idx, holder& child_value) const;
    
    virtual double to_double(holder const& value) const;

    virtual std::string to_string(holder const& h) const;
    virtual void from_string(holder& h, const std::string& str) const;

    virtual void copy(holder const& src, holder& dst) const;

    const std::string& get_type_fqn() const;
    const std::string& get_type_name() const;

    void set_type_fqn(const char *);
    void set_fixed_size(bool fixed);

protected:
    descriptor_base(descriptor_ptr parent = NULL, 
            unsigned int child_index = 0);

    descriptor_ptr m_parent;
    unsigned int m_child_index;
    std::string m_type_fqn;
    std::string m_type_name;

    bool m_fixed_size;
};

struct descriptor_group
{
    descriptor_group();
    virtual ~descriptor_group();

    unsigned int get_descriptor_count() const;
    descriptor_ptr get_descriptor_by_name(
            const std::string& name) const;
    descriptor_ptr get_descriptor_by_index(unsigned int idx) const;
    descriptor_ptr get_descriptor_by_tag(tag_t tag) const;
    const std::string& get_descriptor_name(unsigned int idx) const;
    std::string get_descriptor_name(descriptor_ptr descriptor) const;

    void insert_descriptor(const std::string& name,
        tag_t tag, descriptor_ptr descriptor);

    template< typename T >
    void insert_descriptor(const std::string& name)
    {
        insert_descriptor(name, tag< T >::value(), 
                descriptor< T >::instance());
    }

protected:

    // Data
    typedef std::vector< descriptor_ptr > 
        descriptor_t;
    descriptor_t m_descriptor;

    typedef std::map< std::string, descriptor_ptr > 
        descriptor_by_name_t;
    descriptor_by_name_t m_descriptor_by_name;

    typedef std::map< descriptor_ptr, std::string > 
        name_by_descriptor_t;
    name_by_descriptor_t m_name_by_descriptor;

    typedef std::map< tag_t, descriptor_ptr > 
        descriptor_by_tag_t;
    descriptor_by_tag_t m_descriptor_by_tag;

    typedef std::vector< std::string > 
        names_t;
    names_t m_names;
};

struct connection_descriptor
{
    connection_descriptor();
    connection_descriptor(
            descriptor_group_ptr in_,
            descriptor_group_ptr out_);
    virtual ~connection_descriptor();

    const descriptor_group_ptr in() const;
    const descriptor_group_ptr out() const;

    descriptor_group_ptr in();
    descriptor_group_ptr out();

protected:

    descriptor_group_ptr m_in;
    descriptor_group_ptr m_out;
};

} // namespace core
} // namespace gsim

#endif /* GSIM_CORE_DESCRIPTOR_HPP */

