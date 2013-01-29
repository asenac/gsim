// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * DumpProcessor.hpp
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

#ifndef GSIM_QT_DUMPPROCESSOR_HPP
#define GSIM_QT_DUMPPROCESSOR_HPP

#include <QtCore>

#include <gsim/qt/export.hpp>
#include <gsim/qt/proc/MessageProcessor.hpp>

namespace gsim 
{
namespace qt 
{

class GSIM_QT_DECLSPEC DumpProcessor : public QObject,
    public MessageProcessor
{
    Q_OBJECT
public:

    enum Format
    {
        FORMAT_BINARY,
        FORMAT_TEXT,
        FORMAT_JSON,
        FORMAT_MAX
    };

    struct Config
    {
        bool multipleFiles;
        std::string filePrefix;
        Format format;
        unsigned int suffixLength;
    };

    DumpProcessor(Connection_ptr connection_,
            TypeDescriptor_ptr messageDescriptor_,
            const ReflectivePath_t path_,
            const Config& config);
    ~DumpProcessor();

    void process(Message_ptr msg, Holder hold);

protected:

    void nextFile();

    const Config m_config;
    unsigned int m_currentIndex;
    std::string m_nextFile;
    std::string m_extension;
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_DUMPPROCESSOR_HPP */
