// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * ParametersFromFilesTool.hpp
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

#ifndef GSIM_QT_PARAMETERSFROMFILESTOOL_HPP
#define GSIM_QT_PARAMETERSFROMFILESTOOL_HPP

#include <QtGui>
#include <QList>
#include <fstream>
#include <gsim/qt/export.hpp>
#include <gsim/qt/types.hpp>
#include <gsim/qt/item/ParameterModel.hpp>
#include <gsim/qt/proc/FileLoaderProcessor.hpp>

namespace gsim 
{
namespace qt 
{
class SortableGroup;
class SortableGroupItem;
} // namespace qt

namespace qt 
{

class GSIM_QT_DECLSPEC FilesItem : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QStringList files READ files)
    Q_PROPERTY(int currentFile READ currentFile)
    Q_PROPERTY(int format READ format)
    Q_PROPERTY(bool repeat READ repeat)
public:

    FilesItem(
            Connection_ptr connection,
            TypeDescriptor_ptr reflective,
            const QList< int >& path, 
            QWidget * parent = 0);
    virtual ~FilesItem();

    TypeDescriptor_ptr getReflective() const;

    inline const QList< int >& path() const
    {
        return m_path;
    }

    void save(QVariant& settings);
    void load(const QVariant& settings);

    // Properties
    const QStringList& files() const;
    int currentFile() const;
    int format() const;
    bool repeat() const;

    FileLoaderProcessor_ptr createProcessor();

signals:

    void nextFile(int index);

protected slots:

    void browse();

protected:

    Connection_ptr m_connection;
    TypeDescriptor_ptr m_reflective;
    const QList< int > m_path;

    QStringList m_files;
    QLineEdit * m_filesWidget;
    QComboBox * m_currentFile;
    QComboBox * m_format;
    QCheckBox * m_repeat;
};

class GSIM_QT_DECLSPEC ParametersFromFilesTool : 
    public QWidget
{
    Q_OBJECT
public:

    ParametersFromFilesTool(QWidget * parent = 0);
    virtual ~ParametersFromFilesTool();

    void save(QVariant& settings);
    void load(const QVariant& settings);

    void initialize(
            Connection_ptr connection,
            const QString& messageName,
            TypeDescriptor_ptr reflective);

    void createProcessors(QList< MessageProcessor_ptr >& processors);

public slots:

    FilesItem * createFilesItem(
            TypeDescriptor_ptr,
            const QList< int >& path);

    void deleteFilesItem(
            TypeDescriptor_ptr,
            const QList< int >& path);

    void clear();

protected slots:

    void deleteRequested(gsim::qt::SortableGroupItem *);

protected:
    ParameterModel m_model;
    qt::SortableGroup * m_group;

    typedef QList< FilesItem * > items_t;
    items_t m_items;
    
    Connection_ptr m_connection;
    QString m_messageName;
    TypeDescriptor_ptr m_message;
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_PARAMETERSFROMFILESTOOL_HPP */

