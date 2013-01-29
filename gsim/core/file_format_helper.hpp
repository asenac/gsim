// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * file_format_helper.hpp
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

#ifndef GSIM_CORE_FILE_HELPER_HPP
#define GSIM_CORE_FILE_HELPER_HPP

#include <ostream>
#include <istream>
#include <gsim/core/descriptor_fwd.hpp>
#include <gsim/core/holder.hpp>

namespace gsim 
{
namespace core 
{

enum file_format
{
    FILE_FORMAT_BINARY,
    FILE_FORMAT_TEXT,
    FILE_FORMAT_JSON,
    FILE_FORMAT_MAX
};

class file_format_helper;

class file_format_factory
{
public:

    ~file_format_factory();

    static const file_format_factory* get_instance();

    const file_format_helper* get_helper(file_format format) const;

protected:

    file_format_factory();
};

class file_format_helper
{
public:

    virtual ~file_format_helper();

    virtual bool load(std::istream& is, 
            descriptor_ptr descriptor, 
            holder h) const = 0; 

    virtual bool save(std::ostream& os, 
            descriptor_ptr descriptor, 
            holder h) const = 0; 

protected:

    file_format_helper();
};

class json_file_format_helper : public file_format_helper
{
public:

    virtual ~json_file_format_helper();

    bool load(std::istream& is, 
            descriptor_ptr descriptor, 
            holder h) const; 

    bool save(std::ostream& os, 
            descriptor_ptr descriptor, 
            holder h) const; 

    static const json_file_format_helper* get_instance();

protected:

    json_file_format_helper();
};

class text_file_format_helper : public file_format_helper
{
public:

    virtual ~text_file_format_helper();

    bool load(std::istream& is, 
            descriptor_ptr descriptor, 
            holder h) const; 

    bool save(std::ostream& os, 
            descriptor_ptr descriptor, 
            holder h) const; 

    static const text_file_format_helper* get_instance();

protected:

    text_file_format_helper();
};

class binary_file_format_helper : public file_format_helper  
{
public:

    virtual ~binary_file_format_helper();

    bool load(std::istream& is, 
            descriptor_ptr descriptor, 
            holder h) const; 

    bool save(std::ostream& os, 
            descriptor_ptr descriptor, 
            holder h) const; 

    static const binary_file_format_helper* get_instance();

protected:

    binary_file_format_helper();
};

} // namespace core
} // namespace gsim

#endif /* GSIM_CORE_FILE_HELPER_HPP */

