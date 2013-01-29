// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * DumpProcessor.cpp
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

#include "DumpProcessor.hpp"
#include <gsim/core/file_format_helper.hpp>

#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>

using namespace gsim::qt;

DumpProcessor::DumpProcessor(Connection_ptr connection_,
        TypeDescriptor_ptr messageDescriptor_,
        const ReflectivePath_t path_, 
        const Config& config) :
    MessageProcessor(connection_, messageDescriptor_, path_), m_config(config),
    m_currentIndex(0)
{
    switch(m_config.format)
    {
    case FORMAT_JSON:
        m_extension = ".json";
        break;

    case FORMAT_TEXT:
        m_extension = ".txt";
        break;

    case FORMAT_BINARY:
        m_extension = ".bin";
        break;

    default:
        break;
    }
    
    nextFile();
}

DumpProcessor::~DumpProcessor()
{
}

void DumpProcessor::nextFile()
{
    std::ostringstream oss;

    oss << m_config.filePrefix 
        << std::setfill('0')
        << std::setw(m_config.suffixLength)
        << m_currentIndex
        << m_extension;

    m_nextFile = oss.str();
}

void DumpProcessor::process(Message_ptr msg, Holder hold)
{
    using namespace ::gsim::core;

    std::ios_base::openmode flags = std::ios_base::out;

    const file_format_factory * factory = 
        file_format_factory::get_instance();

    TypeDescriptor_ptr descriptor = hold.get_type_descriptor();

    if (!m_config.multipleFiles)
    {
        flags = flags | std::ios_base::app;
    }

    try 
    {
        switch(m_config.format)
        {
        case FORMAT_JSON:
            {
                std::ofstream out(m_nextFile.c_str(), flags);

                factory->get_helper(FILE_FORMAT_JSON)->save(out, descriptor, hold);
                
                out.close();
            }
            break;

        case FORMAT_TEXT:
            {
                std::ofstream out(m_nextFile.c_str(), flags);

                factory->get_helper(FILE_FORMAT_TEXT)->save(out, descriptor, hold);

                out.close();
            }
            break;

        case FORMAT_BINARY:
            {
                flags = flags | std::ios::binary;

                std::ofstream out(m_nextFile.c_str(), flags);

                factory->get_helper(FILE_FORMAT_BINARY)->save(out, descriptor, hold);

                out.close();
            }
            break;

        default:
            break;
        }
    } 
    catch(...) 
    {
    }

    if (m_config.multipleFiles)
    {
        m_currentIndex++;
        nextFile();
    }
}


