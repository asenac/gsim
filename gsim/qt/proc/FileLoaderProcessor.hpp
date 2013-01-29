// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * FileLoaderProcessor.hpp
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

#ifndef GSIM_QT_FILELOADERPROCESSOR_HPP
#define GSIM_QT_FILELOADERPROCESSOR_HPP

#include <fstream>
#include <QtCore>
#include <gsim/qt/export.hpp>
#include <gsim/qt/proc/MessageProcessor.hpp>

namespace gsim 
{
namespace qt 
{

class FileLoaderProcessor;

typedef boost::shared_ptr< FileLoaderProcessor > 
    FileLoaderProcessor_ptr;

class GSIM_QT_DECLSPEC FileLoaderProcessor :
    public QObject, public MessageProcessor
{
    Q_OBJECT
public:

    FileLoaderProcessor(
        Connection_ptr connection,
        TypeDescriptor_ptr messageDescriptor,
        const QList< int >& path,
        const QStringList& files,
        int currentFile,
        const int format,
        const bool repeat);

    virtual ~FileLoaderProcessor();
    
    void process(Message_ptr request, Holder holder);

signals:

    void nextFile(int index);

protected:

    void openFile();

    const QStringList m_files;
    int m_currentFile;
    const int m_format;
    const bool m_repeat;

    typedef boost::shared_ptr< std::ifstream > ifstream_ptr;
    ifstream_ptr m_currentIStream;
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_FILELOADERPROCESSOR_HPP */

