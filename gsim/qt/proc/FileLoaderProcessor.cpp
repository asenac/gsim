// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * FileLoaderProcessor.cpp
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

#include "FileLoaderProcessor.hpp"
#include <gsim/core/file_format_helper.hpp>
#include <iostream>

using namespace gsim::qt;

FileLoaderProcessor::FileLoaderProcessor(
        Connection_ptr connection,
        TypeDescriptor_ptr messageDescriptor,
        const QList< int >& path,
        const QStringList& files,
        int currentFile,
        const int format,
        const bool repeat) :
    MessageProcessor(connection, messageDescriptor, path),
    m_files(files), 
    m_currentFile(currentFile), 
    m_format(format), 
    m_repeat(repeat)
{
}

FileLoaderProcessor::~FileLoaderProcessor()
{
}

void FileLoaderProcessor::process(Message_ptr request, Holder holder)
{
    using namespace ::gsim::core;

    // std::cout << __FUNCTION__  << std::endl;

    if (m_files.size() == 0)
        throw QString("No files.");

    file_format_factory const * factory = 
        file_format_factory::get_instance();

    file_format_helper const * helper = 
        factory->get_helper(static_cast< file_format >(m_format));

    TypeDescriptor_ptr reflective = holder.get_type_descriptor();

    if (helper)
    {
        if (!m_currentIStream || !m_currentIStream->good())
        {
            openFile();
        }

        bool end = false;
        int i = 0;
        while (m_currentIStream && !end 
                && i++ < m_files.size())
        {
            end = helper->load(
                    *m_currentIStream, 
                    reflective, 
                    holder);

            if (!end) openFile();
        }
    }
    else
    {
        std::cerr << "Invalid file helper!" << std::endl;
    }
}

void FileLoaderProcessor::openFile()
{
    m_currentIStream.reset();

    if (m_files.size() > 0 && 
            (m_currentFile < m_files.size() || m_repeat))
    {
        if (m_repeat && m_currentFile == m_files.size())
        {
            m_currentFile = 0;
        }
    
        if (m_currentFile < m_files.size())
        {
            const QString& file = m_files.at(m_currentFile);

            m_currentIStream.reset(new std::ifstream(
                        file.toStdString().c_str()));

            if (!m_currentIStream->good())
                throw QString("Invalid file %1").arg(file);

            // open a new file
            emit nextFile(m_currentFile);

            m_currentFile++;
        }
    }
    else if (!m_repeat)
    {
        throw QString("No more files.");
    }
}

