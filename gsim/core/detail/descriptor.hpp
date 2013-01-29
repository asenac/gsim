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

#ifndef GSIM_CORE_DETAIL_DESCRIPTOR_HPP
#define GSIM_CORE_DETAIL_DESCRIPTOR_HPP

#include <gsim/adapted.hpp>
#include <gsim/core/detail/adapted.hpp>
#include <gsim/core/descriptor.hpp>
#include <gsim/core/mpl.hpp>

namespace gsim 
{
namespace core 
{
namespace detail 
{

template< typename T >
struct string_converter;

template< typename T, unsigned int N >
struct string_converter< T [N] >
{
    typedef T type[N];
    static std::string to(const type& t_)
    {
        return std::string();
    }
    static void from(const std::string&, type&)
    {
    }
};

template< std::size_t N >
struct string_converter< char [N] >
{
    typedef char type[N];
    static std::string to(const type& t_)
    {
        return std::string(t_, N);
    }
    static void from(const std::string& s_, type& t_)
    {
        const std::size_t min = std::min(N, s_.size() + 1);
        std::memcpy(t_, s_.c_str(), min);
    }
};

template< typename T >
struct bool_descriptor : public descriptor_base
{
    bool_descriptor(descriptor_ptr parent = NULL, unsigned int idx = 0);
    
    descriptor_ptr get_type_descriptor() const;

    static descriptor_ptr instance();
    
    bool is_primitive() const;
    
    holder create_holder() const;

    descriptor_type get_type() const;
    
    void copy(holder const& src, holder& dst) const;

    std::string to_string(holder const& h) const;

    void from_string(holder& h, const std::string& str) const;
};

template< typename T >
struct primitive_descriptor : public descriptor_base
{
    primitive_descriptor(descriptor_ptr parent = NULL, 
            unsigned int idx = 0);

    descriptor_ptr get_type_descriptor() const;

    static descriptor_ptr instance();

    bool is_primitive() const;

    descriptor_type get_type() const;
    
    holder create_holder() const;

    double to_double(holder const& value) const;
    
    void copy(holder const& src, holder& dst) const;

    std::string to_string(holder const& h) const;

    void from_string(holder& h, const std::string& str) const;
};

template< typename T >
struct array_descriptor : public descriptor_base
{
    typedef typename boost::remove_bounds < T >::type slice_t;
    typedef descriptor < slice_t > slice_descriptor_t;

    static const size_t size = sizeof(T) / sizeof(slice_t);

    array_descriptor(descriptor_ptr parent = NULL, unsigned int idx = 0);

    descriptor_ptr get_type_descriptor() const;

    static descriptor_ptr instance();

    unsigned int get_children_count() const;

    descriptor_ptr get_child(unsigned int idx) const;

    bool is_repeated() const;

    descriptor_ptr get_slice() const;

    descriptor_type get_type() const;

    // Dynamic information

    holder create_holder() const;

    unsigned int get_length(holder const& value) const;

    holder get_child_value(holder& value, unsigned int idx) const;

    void copy(holder const& src, holder& dst) const;

    std::string to_string(holder const& h) const;

    void from_string(holder& h, const std::string& str) const;

    slice_descriptor_t m_slice;
};

template< typename T >
struct string_descriptor : public descriptor_base
{
    string_descriptor(descriptor_ptr parent = NULL,
            unsigned int idx = 0);

    descriptor_ptr get_type_descriptor() const;

    static descriptor_ptr instance();

    bool is_variable_length() const;

    bool is_primitive() const;

    descriptor_type get_type() const;

    holder create_holder() const;

    std::string to_string(holder const& h) const;

    void from_string(holder& h, const std::string& str) const;
    
    void copy(holder const& src, holder& dst) const;
};

template< typename T >
struct sequence_descriptor : public descriptor_base
{
    typedef typename adapted::slice < T >::type slice_t;

    sequence_descriptor(descriptor_ptr parent = NULL,
            unsigned int idx = 0);
    
    ~sequence_descriptor();

    descriptor_ptr get_type_descriptor() const;

    static descriptor_ptr instance();

    descriptor_ptr get_child(unsigned int idx) const;

    bool is_repeated() const;
    bool is_variable_length() const;
    
    descriptor_type get_type() const;

    descriptor_ptr get_slice() const;

    // Dynamic information
    holder create_holder() const;

    unsigned int get_length(holder const& value) const;

    void set_length(holder& value, unsigned int length) const;

    holder get_child_value(holder& value, 
        unsigned int idx) const;

    void copy(holder const& src, holder& dst) const;

    descriptor_ptr m_slice;
};

typedef std::vector< dynamic_descriptor_ptr > 
    descriptor_children;

struct accessor_base
{
    virtual holder get(holder& parent) const = 0;
    virtual void set(holder& parent, holder& value) const = 0;
    virtual ~accessor_base() {}
};

typedef boost::shared_ptr< accessor_base > accessor_ptr;

template< typename T >
struct struct_descriptor : public descriptor_base
{
    static const std::size_t members_count = 
        boost::fusion::result_of::size< T >::value;
    typedef boost::mpl::range_c< size_t, 0, members_count > 
        members_range_t;

    struct_descriptor(descriptor_ptr parent = NULL, 
            unsigned int idx = 0);

    descriptor_ptr get_type_descriptor() const;

    static descriptor_ptr instance();

    unsigned int get_children_count() const;

    const char * get_child_name(unsigned int idx) const;
    
    descriptor_ptr get_child(unsigned int idx) const;
    
    descriptor_ptr get_child_by_name(const char *) const;

    unsigned int get_child_index(const char *) const;

    descriptor_type get_type() const;

    // Dynamic information
    holder create_holder() const;

    holder get_child_value(holder& value, 
        unsigned int idx) const;

    void copy(holder const& src, holder& dst) const;

    static struct_descriptor const * get_instance();

    // Data
    descriptor_children m_children;
    std::vector< const char * > m_child_names;
    std::vector< accessor_ptr > m_accessors;
    typedef std::map< std::string, unsigned int > children_by_name_t;
    children_by_name_t m_children_by_name;
};

template< typename T >
struct enum_descriptor : public descriptor_base
{
    typedef adapted::enumeration< T > adapted_t;

    enum_descriptor(descriptor_ptr parent = NULL, 
            unsigned int idx = 0);

    descriptor_ptr get_type_descriptor() const;

    static descriptor_ptr instance();

    bool is_enum() const;

    descriptor_type get_type() const;

    unsigned int get_children_count() const;

    const char * get_child_name(unsigned int idx) const;
    
    unsigned int get_child_index(const char *) const;

    /**
     * @brief Obtain the value associated to a literal using its index. 
     *
     * @param value A null holder.
     * @param idx The index of the literal.
     *
     * @return A holder of this type. 
     */
    holder get_child_value(holder& value, unsigned int idx) const;

    holder create_holder() const;

    void copy(holder const& src, holder& dst) const;
};

template< typename T >
struct unsupported_type : public descriptor_base
{
    unsupported_type(descriptor_ptr parent = NULL, 
            unsigned int idx = 0);

    descriptor_ptr get_type_descriptor() const;

    static descriptor_ptr instance();
};

/**
 * @brief Calculates the descriptor type for a type Y based on a type T.
 *
 * @tparam T A basic type.
 * @tparam Y Must have the same basic usage than T.
 */
template< typename T, typename Y = T >
struct calculate_descriptor
{
    typedef typename 
        // if
        empl::eval_if_identity< empl::is_bool< T >, 
            bool_descriptor< Y >,
        // else if
        empl::eval_if_identity< boost::is_arithmetic< T >, 
            primitive_descriptor< Y >,
        // else if
        empl::eval_if_identity< boost::is_array< T >, 
            array_descriptor< Y >,
        // else if
        empl::eval_if_identity< boost::is_enum< T >, 
            enum_descriptor< Y >,
        // else if
        empl::eval_if_identity< empl::is_struct< T >, 
            struct_descriptor< Y >,
        // else
            boost::mpl::identity< unsupported_type< Y > >
        > > > > >::type type;
};

template< typename SliceT >
struct calculate_descriptor< std::vector< SliceT >, std::vector< SliceT > >
{
    typedef sequence_descriptor< std::vector< SliceT > > type;
};

template< >
struct calculate_descriptor< std::string, std::string >
{
    typedef string_descriptor< std::string > type;
};

template < typename T, typename Y >
descriptor_base * create_descriptor(const T& t_, const Y& y_, 
        descriptor_ptr parent, unsigned int idx)
{
    return new typename calculate_descriptor< T, Y >::type (parent, idx);
}

} // namespace detail

template < typename T >
struct descriptor : public detail::calculate_descriptor< T >::type
{
    typedef typename detail::calculate_descriptor< T >::type base_t;

    descriptor(descriptor_ptr parent = NULL,
            unsigned int idx = 0) : base_t(parent, idx) 
    {}
};

} // namespace core
} // namespace gsim

#endif /* GSIM_CORE_DETAIL_DESCRIPTOR_HPP */

