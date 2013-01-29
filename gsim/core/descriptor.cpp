// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * descriptor.cpp
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

#include "descriptor.hpp"
#include "descriptor_utils.hpp"
#include <gsim/core/detail/descriptor_explicit.ipp>
#include <cassert>
#include <cstring>
#include <algorithm>
#include <limits>

// Explicit instantiation
namespace gsim 
{
namespace core 
{
namespace detail 
{

template class bool_descriptor< bool >;
template class primitive_descriptor< short >;
template class primitive_descriptor< unsigned short >;
template class primitive_descriptor< char >;
template class primitive_descriptor< unsigned char >;
template class primitive_descriptor< int32_t >;
template class primitive_descriptor< uint32_t >;
template class primitive_descriptor< int64_t >;
template class primitive_descriptor< uint64_t >;
template class primitive_descriptor< float >;
template class primitive_descriptor< double >;

} // namespace detail
} // namespace core
} // namespace gsim

using namespace gsim::core;

descriptor_base::descriptor_base(descriptor_ptr parent,
        unsigned int child_index) : 
    m_parent(parent), m_child_index(child_index), m_fixed_size(true)
{
}

descriptor_base::~descriptor_base()
{
}

bool descriptor_base::is_fixed_size() const
{
    return m_fixed_size;
}

void descriptor_base::set_fixed_size(bool fixed)
{
    m_fixed_size = fixed;
}

const std::string& descriptor_base::get_type_fqn() const
{
    return m_type_fqn;
}

const std::string& descriptor_base::get_type_name() const
{
    return m_type_name;
}

void descriptor_base::set_type_fqn(const char * fqn)
{
    m_type_fqn = fqn;

    std::size_t pos = m_type_fqn.find_last_of(':');

    m_type_name = 
        (pos == std::string::npos)? 
            m_type_fqn:
            m_type_fqn.substr(pos + 1);
}

descriptor_ptr descriptor_base::get_type_descriptor() const
{
    return NULL;
}

descriptor_ptr descriptor_base::get_parent() const 
{ 
    return m_parent; 
}

unsigned int descriptor_base::get_child_index() const 
{ 
    return m_child_index; 
}

unsigned int descriptor_base::get_children_count() const 
{ 
    return 0; 
}

const char * descriptor_base::get_child_name(unsigned int idx) const 
{ 
    return 0;
}

descriptor_ptr descriptor_base::get_child(unsigned int idx) const
{
    return 0;
}

descriptor_ptr descriptor_base::get_child_by_name(const char *) const
{
    return 0;
}

unsigned int descriptor_base::get_child_index(const char *) const
{
    return 0;
}

bool descriptor_base::is_repeated() const        { return false; }
bool descriptor_base::is_variable_length() const { return false; }
bool descriptor_base::is_primitive() const       { return false; }
bool descriptor_base::is_enum() const            { return false; }

descriptor_type descriptor_base::get_type() const
{
    return TYPE_INVALID;
}

descriptor_ptr descriptor_base::get_slice() const
{
    return 0;
}

// Dynamic information

holder descriptor_base::create_holder() const
{
    return holder();
}

unsigned int descriptor_base::get_length(holder const& value) const
{
    return 0;
}

void descriptor_base::set_length(holder& value, unsigned int length) const
{
}

holder descriptor_base::get_child_value(holder& value, 
    unsigned int idx) const
{
    return holder();
}

void descriptor_base::set_child_value(holder& value, 
    unsigned int idx, holder& child_value) const
{
    // Default implementation
    if (idx < get_children_count())
    {
        descriptor_ptr ch = get_child(idx);
        core::holder chv = get_child_value(value, idx);

        ch->copy(child_value, chv);
    }
}

double descriptor_base::to_double(holder const& value) const
{
    return 0.0;
}

std::string descriptor_base::to_string(holder const& value) const
{
    return std::string();
}

void descriptor_base::from_string(holder& value, 
        const std::string& str) const
{
}

void descriptor_base::copy(holder const & src, holder& dst) const
{
}

// Descriptor Group

void descriptor_group::insert_descriptor(
        const std::string& name,
        tag_t tag, descriptor_ptr descriptor)
{
    m_names.push_back(name);
    m_descriptor.push_back(descriptor);

    // Maps
    m_descriptor_by_name.insert(std::make_pair(name, descriptor));
    m_name_by_descriptor.insert(std::make_pair(descriptor, name));
    m_descriptor_by_tag.insert(std::make_pair(tag, descriptor));
}

descriptor_group::descriptor_group()
{
}

descriptor_group::~descriptor_group()
{
}

unsigned int descriptor_group::get_descriptor_count() const
{
    return m_descriptor.size();
}

descriptor_ptr 
descriptor_group::get_descriptor_by_index(
        unsigned int idx) const
{
    return m_descriptor[idx];
}

descriptor_ptr 
descriptor_group::get_descriptor_by_name(
        const std::string& name) const
{
    descriptor_by_name_t::const_iterator it;
    it = m_descriptor_by_name.find(name);

    if (it != m_descriptor_by_name.end())
        return it->second;

    return NULL;
}

descriptor_ptr 
descriptor_group::get_descriptor_by_tag(
        tag_t tag) const
{
    descriptor_by_tag_t::const_iterator it;
    it = m_descriptor_by_tag.find(tag);

    if (it != m_descriptor_by_tag.end())
        return it->second;

    return NULL;
}

const std::string& 
descriptor_group::get_descriptor_name(unsigned int idx) const
{
    return m_names[idx];
}

std::string 
descriptor_group::get_descriptor_name(descriptor_ptr descriptor) const
{
    name_by_descriptor_t::const_iterator it = 
        m_name_by_descriptor.find(descriptor);

    if (it != m_name_by_descriptor.end())
    {
        return it->second;
    }

    return std::string();
}

// 
// Connection descriptor
//

connection_descriptor::connection_descriptor() :
    m_in(new descriptor_group), m_out(new descriptor_group)
{
}

connection_descriptor::connection_descriptor(
        descriptor_group_ptr in_,
        descriptor_group_ptr out_) :
    m_in(in_), m_out(out_)
{
}

connection_descriptor::~connection_descriptor()
{
}

const descriptor_group_ptr connection_descriptor::in() const
{
    return m_in;
}

const descriptor_group_ptr connection_descriptor::out() const
{
    return m_out;
}

descriptor_group_ptr connection_descriptor::in()
{
    return m_in;
}

descriptor_group_ptr connection_descriptor::out()
{
    return m_out;
}

namespace
{

typedef std::vector< std::vector< int > > Tmatrix;

int distance(const std::string source, const std::string& target) 
{
    const int m = target.length();
    const int n = source.length();

    if (n == 0) return m;
    
    if (m == 0) return n;

    Tmatrix matrix(n+1); //the matrix

    // resize the rows in the 2nd dimension. Unfortunately C++ doesn't
    // allow the allocation on declaration of 2nd dimension of the matrix directly

    for (int i = 0; i <= n; i++) 
        matrix[i].resize(m+1);

    // Step 2

    for (int i = 0; i <= n; i++) 
        matrix[i][0] = i;

    for (int j = 0; j <= m; j++) 
        matrix[0][j] = j;

    // Step 3
    for (int i = 1; i <= n; i++) 
    {
        const char s_i = source[i-1];

        // Step 4
        for (int j = 1; j <= m; j++) 
        {

            const char t_j = target[j-1];

            // Step 5
            int cost = (s_i == t_j)? 0: 1;

            // Step 6
            const int diag = matrix[i-1][j-1];
            const int left = matrix[i][j-1];
            const int above = matrix[i-1][j];

            int cell = std::min(above + 1, 
                    std::min(left + 1, diag + cost));

            // Step 6A: Cover deletion,
            // insertion, substitution and transposition as well.
            if (i > 2 && j > 2) 
            {
                int trans = matrix[i-2][j-2]+1;
                if (source[i-2] != t_j) trans++;
                if (s_i != target[j-2]) trans++;
                if (cell > trans) cell = trans;
            }

            matrix[i][j] = cell;
        }
    }

    // The final step
    return matrix[n][m];
}

} // namespace

// Utilities
namespace gsim 
{
namespace core 
{
namespace utils 
{

inline void reflective_copy_same_type(holder src, holder dst)
{
    descriptor_ptr dSrc = src.get_type_descriptor();
    descriptor_ptr dDst = dst.get_type_descriptor();

    switch (dSrc->get_type())
    {
        case TYPE_STRUCT:
            {
                const unsigned int count = dSrc->get_children_count();
                const unsigned int dstCount = dDst->get_children_count();

                for (unsigned int i = 0; i < count; i++) 
                {
                    const char * name = dSrc->get_child_name(i);
                    unsigned int dstIndex = dDst->get_child_index(name);

                    // Si no lo ha encontrado con el mismo nombre busca el
                    // más cercano
                    if (dstIndex >= dstCount)
                    {
                        dstIndex = nearest_child(dDst, name);
                    }

                    // Comprueba si tiene un hijo con ese nombre
                    if (dstIndex < dstCount)
                    {
                        // Copia recursiva
                        holder srcChild(dSrc->get_child_value(src, i));
                        holder dstChild(dDst->get_child_value(dst, dstIndex));

                        reflective_copy(srcChild, dstChild);
                    }
                }
            }
            break;

        case TYPE_STRING:
            dDst->from_string(dst, dSrc->to_string(src));
            break;

        case TYPE_BOOL:
            dst.to_value< bool >() = src.to_value< bool >();
            break;
        case TYPE_OCTET:
            dst.to_value< unsigned char >() = src.to_value< unsigned char >();
            break;
        case TYPE_CHAR:
            dst.to_value< char >() = src.to_value< char >();
            break;
        case TYPE_SHORT:
            dst.to_value< short >() = src.to_value< short >();
            break;
        case TYPE_USHORT:
            dst.to_value< unsigned short >() = src.to_value< unsigned short >();
            break;
        case TYPE_LONG:
            dst.to_value< int32_t >() = src.to_value< int32_t >();
            break;
        case TYPE_ULONG:
            dst.to_value< uint32_t >() = src.to_value< uint32_t >();
            break;
        case TYPE_LONGLONG:
            dst.to_value< int64_t >() = src.to_value< int64_t >();
            break;
        case TYPE_ULONGLONG:
            dst.to_value< uint64_t >() = src.to_value< uint64_t >();
            break;

        case TYPE_ENUM:
            {
                bool copied = false;

                // Busca primero el mismo literal.
                // busca el literal más cercano.
                
                const char * literal = get_enum_literal(src);
                
                if (literal)
                {
                    unsigned int dstIndex = dDst->get_child_index(literal);

                    if (dstIndex >= dDst->get_children_count())
                    {
                        dstIndex = nearest_child(dDst, literal);
                    }

                    if (dstIndex < dDst->get_children_count())
                    {
                        holder tmp(dDst->get_child_value(dst, dstIndex));
                        dDst->copy(tmp, dst);
                        copied = true;
                    }
                }

                if (!copied)
                {
                    // En último caso copiar el valor.
                    dst.to_value< int32_t >() = src.to_value< int32_t >();
                }
            }
            break;
        default:
            break;
    }
}

inline void reflective_copy_repeated(holder src, holder dst)
{
    descriptor_ptr dSrc = src.get_type_descriptor();
    descriptor_ptr dDst = dst.get_type_descriptor();

    unsigned int length = dSrc->get_length(src);

    if (!dDst->is_variable_length())
    {
        length = std::min(length, dDst->get_length(dst));
    }
    else
    {
        dDst->set_length(dst, length);
    }

    for (unsigned int i = 0; i < length; i++) 
    {
        holder srcChild(dSrc->get_child_value(src, i));
        holder dstChild(dDst->get_child_value(dst, i));

        reflective_copy(srcChild, dstChild);
    }
}

inline void reflective_copy_primitive(holder src, holder dst)
{
    descriptor_ptr dSrc = src.get_type_descriptor();
    descriptor_ptr dDst = dst.get_type_descriptor();

#define GSIM_CONVERT_PRIMITIVE(SRC)  \
    { \
        switch (dDst->get_type()) \
        { \
        case TYPE_BOOL: \
            dst.to_value< bool >() = (SRC) src.to_value< SRC >(); \
            break; \
        case TYPE_OCTET: \
            dst.to_value< unsigned char >() = (SRC) src.to_value< SRC >(); \
            break; \
        case TYPE_CHAR: \
            dst.to_value< char >() = (SRC) src.to_value< SRC >(); \
            break; \
        case TYPE_SHORT: \
            dst.to_value< short >() = (SRC) src.to_value< SRC >(); \
            break; \
        case TYPE_USHORT: \
            dst.to_value< unsigned short >() = (SRC) src.to_value< SRC >(); \
            break; \
        case TYPE_ENUM: \
        case TYPE_LONG: \
            dst.to_value< int32_t >() = (SRC) src.to_value< SRC>(); \
            break; \
        case TYPE_ULONG: \
            dst.to_value< uint32_t >() = (SRC) src.to_value< SRC >(); \
            break; \
        case TYPE_LONGLONG: \
            dst.to_value< int64_t >() = (SRC) src.to_value< SRC >(); \
            break; \
        case TYPE_ULONGLONG: \
            dst.to_value< uint64_t >() = (SRC) src.to_value< SRC >(); \
            break; \
        default: \
            break; \
        } \
    } \
    /***/

    switch (dSrc->get_type())
    {
        case TYPE_BOOL:
            GSIM_CONVERT_PRIMITIVE(bool);
            break;
        case TYPE_OCTET:
            GSIM_CONVERT_PRIMITIVE(unsigned char);
            break;
        case TYPE_CHAR:
            GSIM_CONVERT_PRIMITIVE(char);
            break;
        case TYPE_SHORT:
            GSIM_CONVERT_PRIMITIVE(short);
            break;
        case TYPE_USHORT:
            GSIM_CONVERT_PRIMITIVE(unsigned short);
            break;
        case TYPE_ENUM:
        case TYPE_LONG:
            GSIM_CONVERT_PRIMITIVE(int32_t);
            break;
        case TYPE_ULONG:
            GSIM_CONVERT_PRIMITIVE(uint32_t);
            break;
        case TYPE_LONGLONG:
            GSIM_CONVERT_PRIMITIVE(int64_t);
            break;
        case TYPE_ULONGLONG:
            GSIM_CONVERT_PRIMITIVE(uint64_t);
            break;
        default:
            break;
    }

#undef GSIM_CONVERT_PRIMITIVE
}

void reflective_copy(holder src, holder dst)
{
    assert(src.is_valid() && dst.is_valid());

    descriptor_ptr dSrc = src.get_type_descriptor();
    descriptor_ptr dDst = dst.get_type_descriptor();

    // Optimization
    if (dSrc == dDst)
        dDst->copy(src, dst);
    else
    {
        if (dSrc->is_repeated() && dDst->is_repeated())
        {
            reflective_copy_repeated(src, dst);
        }
        else if (dSrc->get_type() == dDst->get_type())
        {
            // Same kind of type
            reflective_copy_same_type(src, dst);
        }
        else if (dSrc->is_repeated() && dDst->get_type() == TYPE_STRUCT)
        {
            unsigned int min = std::min(dSrc->get_length(src),
                    dDst->get_children_count());

            for (unsigned int i = 0; i < min; i++) 
            {
                holder srcChild(dSrc->get_child_value(src, i));
                holder dstChild(dDst->get_child_value(dst, i));

                reflective_copy(srcChild, dstChild);
            }
        }
        else if (dDst->is_repeated() && dSrc->get_type() == TYPE_STRUCT)
        {
            unsigned int min = std::min(dDst->get_length(src),
                    dSrc->get_children_count());

            for (unsigned int i = 0; i < min; i++) 
            {
                holder srcChild(dSrc->get_child_value(src, i));
                holder dstChild(dDst->get_child_value(dst, i));

                reflective_copy(srcChild, dstChild);
            }
        }
        else if ((dDst->is_primitive() || dDst->is_enum()) &&
                (dSrc->is_primitive() || dSrc->is_enum()))
        {
            reflective_copy_primitive(src, dst);
        }
        else if ((dSrc->is_primitive() && dDst->get_type() == TYPE_STRING) ||
                (dDst->is_primitive() && dSrc->get_type() == TYPE_STRING))
        {
            const std::string sSrc = dSrc->to_string(src);
            dDst->from_string(dst, sSrc);
        }
        else
        {
            // TODO decidir por donde avanzar
            // NOTE yo creo que el esfuerzo por copiar ya ha sido suficiente,
            // aunque quedan unos pocos casos más:
            // - string <-> enumerado
            // - string <-> array de caracteres
        }
    }
}

unsigned int nearest_child(descriptor_ptr descriptor, 
        const std::string& name)
{
    const unsigned int count = descriptor->get_children_count();
    unsigned int res = count;
    int distance = std::numeric_limits< int >::max();

    for (unsigned int i = 0; i < count; i++) 
    {
        int current_distance = ::distance(name, 
                descriptor->get_child_name(i));

        if (current_distance < distance)
        {
            res = i;
            distance = current_distance;
        }
    }
    
    // TODO Umbral de distancia: Si son demasiado distantes descartarla.

    return res;
}

const char * get_enum_literal(holder src)
{
    descriptor_ptr descriptor = src.get_type_descriptor();

    assert(src.is_valid() && descriptor && descriptor->get_type() == TYPE_ENUM);

    const unsigned int count = 
        descriptor->get_children_count();

    // Obtiene el indice para el valor indicado
    for (unsigned int i = 0; i < count; i++) 
    {
        core::holder tmp(descriptor->get_child_value(src, i));

        if (src.to_value< int32_t >() ==
                tmp.to_value< int32_t >())
        {
            return descriptor->get_child_name(i);
        }
    }

    return NULL;
}

std::size_t get_fixed_size(descriptor_ptr d)
{
#define GSIM_PRIMITIVE_SIZE(X, Y) \
      case TYPE_ ## X : return sizeof(Y); \
      /**/

    switch(d->get_type())
    {
        case TYPE_ENUM:
        GSIM_PRIMITIVE_SIZE(LONG, int32_t);
        GSIM_PRIMITIVE_SIZE(ULONG, uint32_t);
        GSIM_PRIMITIVE_SIZE(LONGLONG, int64_t);
        GSIM_PRIMITIVE_SIZE(ULONGLONG, uint64_t);
        GSIM_PRIMITIVE_SIZE(SHORT, short);
        GSIM_PRIMITIVE_SIZE(USHORT, unsigned short);
        GSIM_PRIMITIVE_SIZE(FLOAT, float);
        GSIM_PRIMITIVE_SIZE(DOUBLE, double);
        GSIM_PRIMITIVE_SIZE(OCTET, unsigned char);
        GSIM_PRIMITIVE_SIZE(CHAR, char);

        case TYPE_ARRAY:
            {
                holder dummy;
                const unsigned int count = d->get_length(dummy);
                return count * get_fixed_size(d->get_slice());
            }
            break;

        case TYPE_STRUCT:
            {
                std::size_t res = 0;

                const unsigned int count = d->get_children_count();

                for (unsigned int i = 0; i < count; i++) 
                {
                    res += get_fixed_size(d->get_child(i));
                }

                return res;
            }
            break;

        default:
            break;
    }

    return 0;
}

std::size_t calculate_size(holder h)
{
    assert(h.is_valid());
    
    descriptor_ptr d = h.get_type_descriptor();

    std::size_t res = 0;

    if (d->is_fixed_size())
    {
        res = get_fixed_size(d);
    }
    else
    {
        switch(d->get_type())
        {
        case TYPE_STRUCT:
            {
                const unsigned int count = d->get_children_count();

                for (unsigned int i = 0; i < count; i++) 
                {
                    holder ch(d->get_child_value(h, i));
                    res += calculate_size(ch);
                }
            }
            break;
        case TYPE_ARRAY:
        case TYPE_SEQUENCE:
            {
                const unsigned int count = d->get_length(h);

                for (unsigned int i = 0; i < count; i++) 
                {
                    holder ch(d->get_child_value(h, i));
                    res += calculate_size(ch);
                }
            }
        default:
            break;
        }
    }

    return res;
}

void initialize(holder h)
{
    assert(h.is_valid());
    
    descriptor_ptr d = h.get_type_descriptor();

    if (d->is_fixed_size())
    {
        std::memset(h.get_buffer(), 0, h.get_sizeof());
    }
    else
    {
        switch(d->get_type())
        {
        case TYPE_ARRAY:
        case TYPE_STRUCT:
            {
                const unsigned int count = d->get_children_count();

                for (unsigned int i = 0; i < count; i++) 
                {
                    holder ch(d->get_child_value(h, i));
                    initialize(ch);
                }
            }
            break;
        case TYPE_SEQUENCE:
            {
                d->set_length(h, 0);
            }
        default:
            break;
        }
    }
}

holder create_initialized_holder(descriptor_ptr descriptor)
{
    assert(descriptor);

    holder h = descriptor->create_holder();
    initialize(h);
    return h;
}

std::string get_member_fqn(descriptor_ptr descriptor)
{
    std::string res;
    descriptor_ptr parent = descriptor->get_parent();
    
    if (parent && parent->get_type() == TYPE_STRUCT)
    {
        res += parent->get_type_fqn();
        res += "::";
        res += parent->get_child_name(descriptor->get_child_index());
    }

    return res;
}

} // namespace utils
} // namespace core
} // namespace gsim

